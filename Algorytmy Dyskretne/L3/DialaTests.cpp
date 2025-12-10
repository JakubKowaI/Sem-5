#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <fstream>
#include <sstream>
#include <chrono>
#include <atomic>
#include <algorithm>
#include <climits>
#include <random>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

using namespace std;

// Funkcja czytająca MemAvailable z /proc/meminfo (kB).
// Jeśli nie uda się odczytać, zwróci 0.
static long get_mem_available_kb() {
    ifstream f("/proc/meminfo");
    if (!f.is_open()) return 0;
    string line;
    while (getline(f, line)) {
        if (line.rfind("MemAvailable:", 0) == 0) {
            istringstream iss(line);
            string key, value, unit;
            if (iss >> key >> value >> unit) {
                try {
                    return stol(value); // kB
                } catch (...) {
                    return 0;
                }
            }
        }
    }
    return 0;
}

// Prosty thread-pool: ogranicza liczbę workerów (równoległych procesów).
// Każdy worker przed wykonaniem polecenia sprawdza, czy jest wystarczająco wolnej pamięci.
class ThreadPool {
    vector<thread> workers;
    queue<string> tasks;
    mutex m;
    condition_variable cv;
    atomic<bool> stop{false};
    long min_free_kb; // minimalna dostępna pamięć (kB) wymagana do startu zadania
    chrono::milliseconds check_interval;
    long per_process_limit_kb; // 0 = no limit (KB)
    int per_process_timeout_sec; // 0 = no timeout (seconds)

    // Uruchamia polecenie w podprocesie (fork+exec). Dzięki temu możemy:
    // - sprawdzić status zakończenia dokładnie (WIFEXITED/WIFSIGNALED)
    // - w child ustawić limity zasobów (setrlimit) jeśli chcemy ograniczyć pamięć procesu
    // - w parent wymusić limit czasu (terminacja po timeout)
    static int run_command_with_limits(const string &cmd, long per_process_limit_kb, int timeout_seconds) {
        pid_t pid = fork();
        if (pid < 0) {
            // fork failed
            cerr << "fork() failed: " << strerror(errno) << "\n";
            return -1;
        }
        if (pid == 0) {
            // child
            if (per_process_limit_kb > 0) {
                struct rlimit rl;
                // per_process_limit_kb is KB, convert to bytes
                rlim_t bytes = (rlim_t)per_process_limit_kb * 1024ULL;
                rl.rlim_cur = rl.rlim_max = bytes;
                // Try to set address space limit and data segment limit
                setrlimit(RLIMIT_AS, &rl);
                setrlimit(RLIMIT_DATA, &rl);
            }

            // Exec via shell to allow complex commands/redirections
            execl("/bin/bash", "bash", "-lc", cmd.c_str(), (char *)NULL);
            // If exec fails:
            _exit(127);
        } else {
            // parent: wait for child with timeout
            int status = 0;
            bool terminated_by_timeout = false;
            auto start = chrono::steady_clock::now();
            while (true) {
                pid_t w = waitpid(pid, &status, WNOHANG);
                if (w == -1) {
                    // error
                    if (errno == EINTR) continue;
                    cerr << "waitpid failed: " << strerror(errno) << "\n";
                    return -1;
                } else if (w == 0) {
                    // still running
                    if (timeout_seconds > 0) {
                        auto now = chrono::steady_clock::now();
                        auto elapsed = chrono::duration_cast<chrono::seconds>(now - start).count();
                        if (elapsed >= timeout_seconds) {
                            // First try graceful termination
                            if (kill(pid, SIGTERM) == 0) {
                                cerr << "Process " << pid << " exceeded timeout (" << timeout_seconds << "s), sent SIGTERM\n";
                            } else {
                                if (errno != ESRCH) // no such process is ok
                                    cerr << "Failed to send SIGTERM to " << pid << ": " << strerror(errno) << "\n";
                            }
                            // wait up to 2 more seconds for graceful exit
                            auto grace_start = chrono::steady_clock::now();
                            bool exited = false;
                            while (true) {
                                pid_t w2 = waitpid(pid, &status, WNOHANG);
                                if (w2 == -1) {
                                    if (errno == EINTR) continue;
                                    break;
                                } else if (w2 == 0) {
                                    auto now2 = chrono::steady_clock::now();
                                    auto gelapsed = chrono::duration_cast<chrono::seconds>(now2 - grace_start).count();
                                    if (gelapsed >= 2) break;
                                    this_thread::sleep_for(chrono::milliseconds(100));
                                    continue;
                                } else {
                                    exited = true;
                                    break;
                                }
                            }
                            if (!exited) {
                                // Force kill
                                if (kill(pid, SIGKILL) == 0) {
                                    cerr << "Process " << pid << " did not exit after SIGTERM, sent SIGKILL\n";
                                } else {
                                    if (errno != ESRCH)
                                        cerr << "Failed to send SIGKILL to " << pid << ": " << strerror(errno) << "\n";
                                }
                            }
                            terminated_by_timeout = true;
                            // now wait for final status
                            pid_t w3;
                            do {
                                w3 = waitpid(pid, &status, 0);
                            } while (w3 == -1 && errno == EINTR);
                            if (w3 == -1) {
                                cerr << "waitpid after kill failed: " << strerror(errno) << "\n";
                                return -1;
                            }
                            break;
                        }
                    }
                    // sleep a bit before next poll
                    this_thread::sleep_for(chrono::milliseconds(100));
                    continue;
                } else {
                    // child exited normally or via signal
                    break;
                }
            }

            // At this point status is filled (unless waitpid loop had an error)
            if (WIFEXITED(status)) {
                int exitcode = WEXITSTATUS(status);
                if (exitcode != 0) {
                    cerr << "Command exited with code " << exitcode << ": " << cmd << "\n";
                }
            } else if (WIFSIGNALED(status)) {
                int sig = WTERMSIG(status);
                const char *sname = strsignal(sig);
                if (terminated_by_timeout) {
                    cerr << "Command was terminated due to timeout. Signal " << sig
                         << (sname ? (string(" (") + sname + ")") : string()) << ": " << cmd << "\n";
                } else {
                    cerr << "Command killed by signal " << sig
                         << (sname ? (string(" (") + sname + ")") : string()) << ": " << cmd << "\n";
                }
            }
            return status;
        }
    }

public:
    // per_process_limit_mb: 0 = no per-process memory limit; otherwise set RLIMIT_AS (MB)
    // per_process_timeout_s: 0 = no timeout
    ThreadPool(size_t num_workers, long min_free_mb, long per_process_limit_mb = 0, int per_process_timeout_s = 0,
               chrono::milliseconds wait_interval = chrono::milliseconds(1000))
        : min_free_kb(min_free_mb * 1024),
          check_interval(wait_interval),
          per_process_limit_kb(per_process_limit_mb * 1024),
          per_process_timeout_sec(per_process_timeout_s)
    {
        for (size_t i = 0; i < num_workers; ++i) {
            workers.emplace_back([this]() {
                while (true) {
                    string task;
                    try {
                        {
                            unique_lock<mutex> lk(this->m);
                            this->cv.wait(lk, [this]() { return this->stop.load() || !this->tasks.empty(); });
                            if (this->stop.load() && this->tasks.empty()) return;
                            task = move(this->tasks.front());
                            this->tasks.pop();
                        }

                        // Czekaj aż będzie wystarczająco wolnej pamięci systemowej
                        while (!this->stop.load()) {
                            long avail = get_mem_available_kb();
                            if (avail == 0) {
                                // Nie udało się odczytać /proc/meminfo; w takim wypadku nie blokujemy za długo
                                break;
                            }
                            if (this->min_free_kb == 0) break; // kontrola wyłączona
                            if (avail >= this->min_free_kb) break;
                            this_thread::sleep_for(this->check_interval);
                        }

                        // Wykonaj polecenie z kontrolą zakończenia procesu i limitami
                        int status = run_command_with_limits(task, this->per_process_limit_kb, this->per_process_timeout_sec);
                        if (status == -1) {
                            cerr << "Failed to run command: " << task << "\n";
                        }
                        // continue to next task regardless of child failure or kill
                    } catch (const std::exception &e) {
                        cerr << "Exception in worker thread: " << e.what() << "\n";
                    } catch (...) {
                        cerr << "Unknown exception in worker thread\n";
                    }
                }
            });
        }
    }

    void enqueue(string cmd) {
        {
            lock_guard<mutex> lk(m);
            tasks.push(move(cmd));
        }
        cv.notify_one();
    }

    void shutdown() {
        stop.store(true);
        cv.notify_all();
        for (thread &w : workers) if (w.joinable()) w.join();
    }

    ~ThreadPool() {
        if (!stop.load()) shutdown();
    }
};

void makeSS(string path){
    ofstream f(path);
    if (f.is_open()) {
        // empty placeholder
    }
}

int main() {
    // Konfiguracja - zmień według potrzeb:
    const size_t MAX_CONCURRENT = 2;            // maksymalna liczba jednoczesnych procesów
    const long MIN_FREE_MB = 20000;             // nie startuj nowych procesów jeśli dostępne < MIN_FREE_MB (MB)
    const long PER_PROCESS_LIMIT_MB = 0;        // jeśli >0: ogranicz pamięć pojedynczego procesu (MB). 0 = brak limitu
    const int PER_PROCESS_TIMEOUT_SEC = 30;     // limit czasu dla procesu w sekundach (30s as requested)

    // MIN_FREE_MB = 0 -> wyłącz kontrolę pamięci; wtedy jedyną barierą jest MAX_CONCURRENT.

    vector<string> families = {"Long-C","Long-n","Random4-C","Random4-n","Square-C","Square-n"};
    vector<pair<int,int>> limits = {{0,15},{10,21},{0,15},{10,21},{0,15},{10,21}};

    ThreadPool pool(MAX_CONCURRENT, MIN_FREE_MB, PER_PROCESS_LIMIT_MB, PER_PROCESS_TIMEOUT_SEC);

    for (size_t i = 0; i < families.size(); ++i) {
        const string &family = families[i];
        cout << "Tworzenie testow dla " << family << "\n";
        for (int j = limits[i].first; j <= limits[i].second; ++j) {
            string base = "ch9-1.1/inputs/" + family + "/" + family + "." + to_string(j) + ".0";
            string ssource= base+".ss";

            ifstream data(ssource);

            if (!data.is_open()) {
                cerr << "Nie moge otworzyc pliku: " << ssource << "\n";
                continue;
            }

            ofstream single("processed/"+family + "." + to_string(j) + ".0.single.ss");
            ofstream random("processed/"+family + "." + to_string(j) + ".0.random.ss");
            ofstream p2pout("processed/"+family + "." + to_string(j) + ".0.p2p");

            char c;
            int minNode=INT_MAX;
            vector<int> random5;
            vector<int> snodes;

            mt19937 mt{
                static_cast<std::mt19937::result_type>(
                std::chrono::steady_clock::now().time_since_epoch().count()
            )    };

            while(data.get(c)){
                if(c=='c'){
                    string temp;
                    getline(data,temp);
                }else if(c=='p'){
                    string aux,sp,ss,z;
                    data>>aux>>sp>>ss>>z;
                    string temp;
                    getline(data,temp);
                }else if(c=='s'){
                    int s;
                    data>>s;
                    snodes.push_back(s);
                    string temp;
                    getline(data,temp);
                }
            }

            int p2pMax=INT_MIN;
            int p2pMin=INT_MAX;
            vector<int> sour;
            vector<int> dest;

            if(j==limits[i].second){
                ifstream p2p(base+".p2p");

                if (!p2p.is_open()) {
                    cerr << "Ostrzezenie: nie moge otworzyc pliku p2p: " << base << ".p2p\n";
                }

                while(p2p.get(c)){
                    if(c=='c'){
                        string temp;
                        getline(p2p,temp);
                    }else if(c=='p'){
                        string aux,sp,ss,z;
                        p2p>>aux>>sp>>ss>>z;
                    }else if(c=='q'){
                        int s,d;
                        p2p>>s>>d;

                        if(s>p2pMax)p2pMax=s;
                        if(d>p2pMax)p2pMax=d;
                        if(s<p2pMin)p2pMin=s;
                        if(d<p2pMin)p2pMin=d;

                        sour.push_back(s);
                        dest.push_back(d);
                    }
                }
                p2pout<<"q "<<p2pMin<<" "<<p2pMax<<endl;

                for(int k=0;k<4;k++){
                    if (!snodes.empty())
                        random5.push_back(snodes[mt()%snodes.size()]);
                    if (!sour.empty() && !dest.empty()) {
                        p2pout<<"q "<<sour[mt()%sour.size()]<<" "<<dest[mt()%dest.size()]<<endl;
                    }
                }
                p2p.close();
                p2pout.close();
            }

            for(int k=0;k<5;k++){
                if (!snodes.empty())
                    random5.push_back(snodes[mt()%snodes.size()]);
            }

            for(int v:snodes){
                if(v<minNode)minNode=v;
            }
            if(minNode==INT_MAX) minNode = 0;
            single<<"s "<<minNode;
            for(int t : random5){
                random<<"s "<<t<<endl;
            }
            single.close();
            random.close();
            data.close();
        }
    }

    for (size_t i = 0; i < families.size(); ++i) {
        const string &family = families[i];
        cout << "Testy dla " << family << "\n";
        for (int j = limits[i].first; j <= limits[i].second; ++j) {
            string base = "ch9-1.1/inputs/" + family + "/" + family + "." + to_string(j) + ".0";
            string diala_cmd_ss_single    = "./diala    -d " + base + ".gr -ss " + "processed/"+family + "." + to_string(j) + ".0.single.ss" + " -oss wyniki/diala/"    + family + "." + to_string(j) + ".0.single.ss.res";
            string diala_cmd_ss_random    = "./diala    -d " + base + ".gr -ss " + "processed/"+family + "." + to_string(j) + ".0.random.ss" + " -oss wyniki/diala/"    + family + "." + to_string(j) + ".0.random.ss.res";
            string diala_cmd_p2p    = "./diala    -d " + base + ".gr -p2p " + "processed/"+family + "." + to_string(j) + ".0.p2p" + " -op2p wyniki/diala/"    + family + "." + to_string(j) + ".0.p2p.res";

            pool.enqueue(diala_cmd_ss_single);
            pool.enqueue(diala_cmd_ss_random);

            if(j==limits[i].second){
                pool.enqueue(diala_cmd_p2p);
            }
        }
    }

    pool.shutdown();
    cout << "Wszystkie zadania ukończone.\n";
    return 0;
}