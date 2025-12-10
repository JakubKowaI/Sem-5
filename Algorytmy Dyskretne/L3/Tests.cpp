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

void makeSS(string path){

}

int main() {
    // Konfiguracja - zmień według potrzeb:
    const size_t MAX_CONCURRENT = 12;   // maksymalna liczba jednoczesnych procesów
    const long MIN_FREE_MB = 15000;    // nie startuj nowych procesów jeśli dostępne < MIN_FREE_MB (MB)
    // MIN_FREE_MB = 0 -> wyłącz kontrolę pamięci; wtedy jedyną barierą jest MAX_CONCURRENT.

    vector<string> families = {"Long-C","Long-n","Random4-C","Random4-n","Square-C","Square-n"};
    vector<pair<int,int>> limits = {{0,15},{10,21},{0,15},{10,21},{0,15},{10,21}};

    ThreadPool pool(MAX_CONCURRENT, MIN_FREE_MB);

    // for (size_t i = 0; i < families.size(); ++i) {
    //     const string &family = families[i];
    //     cout << "Tworzenie testow dla " << family << "\n";
    //     for (int j = limits[i].first; j <= limits[i].second; ++j) {
    //         string base = "ch9-1.1/inputs/" + family + "/" + family + "." + to_string(j) + ".0";
    //         string ssource= base+".ss";

    //         ifstream data(ssource);
            

    //         if (!data.is_open()) {
    //             cerr << "Nie moge otworzyc pliku: " << ssource << "\n";
    //             continue;
    //         }
            
    
    //         ofstream single("processed/"+family + "." + to_string(j) + ".0.single.ss");
    //         ofstream random("processed/"+family + "." + to_string(j) + ".0.random.ss");
    //         ofstream p2pout("processed/"+family + "." + to_string(j) + ".0.p2p");

    //         char c;
    //         int minNode=INT_MAX;
    //         vector<int> random5;
    //         vector<int> snodes;

    //         mt19937 mt{
    //             static_cast<std::mt19937::result_type>(
    //             std::chrono::steady_clock::now().time_since_epoch().count()
    //         )    };
            
    //         while(data.get(c)){
    //             //cerr<<c<< "- hahahahah "<<endl;
    //             if(c=='c'){
    //                 string temp;
    //                 getline(data,temp);
    //                 //cout<<temp<<endl;
    //             }else if(c=='p'){
    //                 string aux,sp,ss,z;
    //                 data>>aux>>sp>>ss>>z;
    //                 //n=stoi(ss);
    //                 //cout<<"SS: "<<ss<<endl;
    //                 string temp;
    //                 getline(data,temp);
    //             }else if(c=='s'){
    //                 //cerr<<endl;
    //                 //if(count>4)break;
    //                 int s;
    //                 data>>s;
    //                 snodes.push_back(s);
    //                 string temp;
    //                 getline(data,temp);
    //             }
    //         }
    //         //cerr<<"snodes size: "<<snodes.size()<<endl;

    //         int p2pMax=INT_MIN;
    //         int p2pMin=INT_MAX;
    //         vector<int> sour;
    //         vector<int> dest;

    //         if(j==limits[i].second){
    //             ifstream p2p(base+".p2p");

    //             if (!p2p.is_open()) {
    //             // tylko ostrzezenie; p2p moze byc opcjonalny w twoim workflow
    //                 cerr << "Ostrzezenie: nie moge otworzyc pliku p2p: " << base << ".p2p\n";
    //             }

    //             while(p2p.get(c)){
    //                 if(c=='c'){
    //                     string temp;
    //                     getline(p2p,temp);
    //                     //cout<<temp<<endl;
    //                 }else if(c=='p'){
    //                     string aux,sp,ss,z;
    //                     p2p>>aux>>sp>>ss>>z;
    //                 }else if(c=='q'){
    //                     int s,d;
    //                     p2p>>s>>d;

    //                     if(s>p2pMax)p2pMax=s;
    //                     if(d>p2pMax)p2pMax=d;
    //                     if(s<p2pMin)p2pMin=s;
    //                     if(d<p2pMin)p2pMin=d;

    //                     sour.push_back(s);
    //                     dest.push_back(d);
    //                     //cout<<s<<" : "<<d<<" : "<<c<<endl;
    //                 }
    //             }
    //             p2pout<<"q "<<p2pMin<<" "<<p2pMax<<endl;

    //             //cerr<<sour.size() << " <--> "<<dest.size()<<endl;

    //             for(int i=0;i<4;i++){
    //                 random5.push_back(snodes[mt()%snodes.size()]);
    //                 p2pout<<"q "<<sour[mt()%sour.size()]<<" "<<dest[mt()%dest.size()]<<endl;
    //             }
    //             p2p.close();
    //             p2pout.close();
    //         }

    //         for(int i=0;i<5;i++){
    //             random5.push_back(snodes[mt()%snodes.size()]);
    //         }

    //         for(int i:snodes){
    //             if(i<minNode)minNode=i;
    //         }
    //         single<<"s "<<minNode;
    //         for(int t : random5){
    //             random<<"s "<<t<<endl;
    //         }
    //         single.close();
    //         random.close();
    //         data.close();
    //     }
    // }

    for (size_t i = 0; i < families.size(); ++i) {
        const string &family = families[i];
        cout << "Testy dla " << family << "\n";
        for (int j = limits[i].first; j <= limits[i].second; ++j) {
            string base = "ch9-1.1/inputs/" + family + "/" + family + "." + to_string(j) + ".0";
            //makeSS(base + ".ss");
            string dijkstra_cmd_ss_single = "./dijkstra -d " + base + ".gr -ss " + "processed/"+family + "." + to_string(j) + ".0.single.ss" + " -oss wyniki/dijkstra/" + family + "." + to_string(j) + ".0.single.ss.res";
            //string diala_cmd_ss_single    = "./diala    -d " + base + ".gr -ss " + "processed/"+family + "." + to_string(j) + ".0.single.ss" + " -oss wyniki/diala/"    + family + "." + to_string(j) + ".0.single.ss.res";
            string radix_cmd_ss_single    = "./radixheap -d " + base + ".gr -ss " + "processed/"+family + "." + to_string(j) + ".0.single.ss" + " -oss wyniki/radixheap/" + family + "." + to_string(j) + ".0.single.ss.res";

            string dijkstra_cmd_ss_random = "./dijkstra -d " + base + ".gr -ss " + "processed/"+family + "." + to_string(j) + ".0.random.ss" + " -oss wyniki/dijkstra/" + family + "." + to_string(j) + ".0.random.ss.res";
            //string diala_cmd_ss_random    = "./diala    -d " + base + ".gr -ss " + "processed/"+family + "." + to_string(j) + ".0.random.ss" + " -oss wyniki/diala/"    + family + "." + to_string(j) + ".0.random.ss.res";
            string radix_cmd_ss_random    = "./radixheap -d " + base + ".gr -ss " + "processed/"+family + "." + to_string(j) + ".0.random.ss" + " -oss wyniki/radixheap/" + family + "." + to_string(j) + ".0.random.ss.res";

            string dijkstra_cmd_p2p = "./dijkstra -d " + base + ".gr -p2p " + "processed/"+family + "." + to_string(j) + ".0.p2p" + " -op2p wyniki/dijkstra/" + family + "." + to_string(j) + ".0.p2p.res";
            //string diala_cmd_p2p    = "./diala    -d " + base + ".gr -p2p " + "processed/"+family + "." + to_string(j) + ".0.p2p" + " -op2p wyniki/diala/"    + family + "." + to_string(j) + ".0.p2p.res";
            string radix_cmd_p2p    = "./radixheap -d " + base + ".gr -p2p " + "processed/"+family + "." + to_string(j) + ".0.p2p" + " -op2p wyniki/radixheap/" + family + "." + to_string(j) + ".0.p2p.res";

            pool.enqueue(dijkstra_cmd_ss_single);
            //pool.enqueue(diala_cmd_ss_single);
            pool.enqueue(radix_cmd_ss_single);
            pool.enqueue(dijkstra_cmd_ss_random);
            //pool.enqueue(diala_cmd_ss_random);
            pool.enqueue(radix_cmd_ss_random);

            if(j==limits[i].second){
                pool.enqueue(dijkstra_cmd_p2p);
                //pool.enqueue(diala_cmd_p2p);
                pool.enqueue(radix_cmd_p2p);
            }
            
        }
    }

    pool.shutdown();
    cout << "Wszystkie zadania ukończone.\n";
    return 0;
}