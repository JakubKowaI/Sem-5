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

using namespace std;

// Funkcja czytająca MemAvailable z /proc/meminfo (kB). 
// Jeśli nie uda się odczytać, zwróci 0.
static long get_mem_available_kb() {
    ifstream f("/proc/meminfo");
    if (!f.is_open()) return 0;
    string line;
    while (getline(f, line)) {
        if (line.rfind("MemAvailable:", 0) == 0) {
            // linia przykładowa: "MemAvailable:   12345678 kB"
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

    static void run_command(const string &cmd) {
        int ret = system(cmd.c_str());
        if (ret != 0) {
            cerr << "Command failed (exit=" << ret << "): " << cmd << "\n";
        }
    }

public:
    ThreadPool(size_t num_workers, long min_free_mb, chrono::milliseconds wait_interval = chrono::milliseconds(1000))
        : min_free_kb(min_free_mb * 1024), check_interval(wait_interval)
    {
        for (size_t i = 0; i < num_workers; ++i) {
            workers.emplace_back([this]() {
                while (true) {
                    string task;
                    {
                        unique_lock<mutex> lk(this->m);
                        this->cv.wait(lk, [this]() { return this->stop.load() || !this->tasks.empty(); });
                        if (this->stop.load() && this->tasks.empty()) return;
                        task = move(this->tasks.front());
                        this->tasks.pop();
                    }

                    // Czekaj aż będzie wystarczająco wolnej pamięci systemowej
                    // (nie ograniczamy pamięci procesu, tylko nie startujemy nowych, gdy system jest już obciążony)
                    while (!this->stop.load()) {
                        long avail = get_mem_available_kb();
                        if (avail == 0) {
                            // Nie udało się odczytać /proc/meminfo; w takim wypadku nie blokujemy za długo
                            break;
                        }
                        if (avail >= this->min_free_kb) break;
                        // czekamy, aż zwolni się pamięć
                        this_thread::sleep_for(this->check_interval);
                    }

                    // Wykonaj polecenie (bez ulimit, bez setrlimit): proces może użyć ile chce (ale nie startujemy go, jeśli pamięć byłaby niewystarczająca)
                    run_command(task);
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


int main() {
    // Konfiguracja - zmień według potrzeb:
    const size_t MAX_CONCURRENT = 12;   // maksymalna liczba jednoczesnych procesów
    const long MIN_FREE_MB = 4000;    // nie startuj nowych procesów jeśli dostępne < MIN_FREE_MB (MB)
    // MIN_FREE_MB = 0 -> wyłącz kontrolę pamięci; wtedy jedyną barierą jest MAX_CONCURRENT.

    vector<string> families = {"Long-C","Long-n","Random4-C","Random4-n","Square-C","Square-n"};
    vector<pair<int,int>> limits = {{0,15},{10,21},{0,15},{10,21},{0,15},{10,21}};

    ThreadPool pool(MAX_CONCURRENT, MIN_FREE_MB);

    for (size_t i = 0; i < families.size(); ++i) {
        const string &family = families[i];
        cout << "Testy dla " << family << "\n";
        for (int j = limits[i].first; j <= limits[i].second; ++j) {
            string base = "ch9-1.1/inputs/" + family + "/" + family + "." + to_string(j) + ".0";

            string dijkstra_cmd_ss = "./dijkstra -d " + base + ".gr -ss " + base + ".ss -oss wyniki/dijkstra/" + family + "." + to_string(j) + ".0.ss.res";
            string diala_cmd_ss    = "./diala    -d " + base + ".gr -ss " + base + ".ss -oss wyniki/diala/"    + family + "." + to_string(j) + ".0.ss.res";
            string radix_cmd_ss    = "./radixheap -d " + base + ".gr -ss " + base + ".ss -oss wyniki/radixheap/" + family + "." + to_string(j) + ".0.ss.res";

            string dijkstra_cmd_p2p = "./dijkstra -d " + base + ".gr -p2p " + base + ".p2p -op2p wyniki/dijkstra/" + family + "." + to_string(j) + ".0.p2p.res";
            string diala_cmd_p2p    = "./diala    -d " + base + ".gr -p2p " + base + ".p2p -op2p wyniki/diala/"    + family + "." + to_string(j) + ".0.p2p.res";
            string radix_cmd_p2p    = "./radixheap -d " + base + ".gr -p2p " + base + ".p2p -op2p wyniki/radixheap/" + family + "." + to_string(j) + ".0.p2p.res";

            pool.enqueue(dijkstra_cmd_ss);
            pool.enqueue(diala_cmd_ss);
            pool.enqueue(radix_cmd_ss);
            pool.enqueue(dijkstra_cmd_p2p);
            pool.enqueue(diala_cmd_p2p);
            pool.enqueue(radix_cmd_p2p);
        }
    }

    pool.shutdown();
    cout << "Wszystkie zadania ukończone.\n";
    return 0;
}