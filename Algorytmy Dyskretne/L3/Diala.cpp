#include "Algorithms.hpp"
#include <unordered_set>

struct node{
    int vertex;
};

// struct kubelek{
//     std::vector<node> nodes;
// };

double OGDiala(std::vector<std::vector<std::pair<int,int>>> & graph,int source,int maxcost){
    auto t0 = std::chrono::high_resolution_clock::now();

    std::vector<long> d(graph.size(),LONG_MAX);
    std::vector<std::unordered_set<int>> kubelki;
    kubelki.resize(graph.size()*maxcost+1);
    d[source]=0;
    kubelki[0].insert(source);

    for (int i = 0; i <= kubelki.size(); i++) {
        while (!kubelki[i].empty()) {
            int u = *kubelki[i].begin();
            kubelki[i].erase(kubelki[i].begin());
            
            if (i > d[u]) continue;
            
            for (auto& it : graph[u]) {
                int v = std::get<0>(it);
                int weight = std::get<1>(it);
                int newDist = d[u] + weight;
                
                if (newDist < d[v]) {
                    
                    if (d[v] != LONG_MAX) {
                        kubelki[d[v]].erase(v);
                    }
                    
                    d[v] = newDist;
                    kubelki[newDist].insert(v);
                }
            }
        }
    }
    
    auto t1 = std::chrono::high_resolution_clock::now();
    double elapsed_seconds = std::chrono::duration<double>(t1 - t0).count();
    //std::cout << "Diala elapsed: " << elapsed_seconds << " s" << std::endl;
    return elapsed_seconds;
}

double Diala(std::vector<std::vector<std::pair<int,int>>> & graph,int source,int maxcost,int n){
    auto t0 = std::chrono::high_resolution_clock::now();

    std::vector<long> d(graph.size(),LONG_MAX);
    std::vector<std::unordered_set<int>> kubelki;
    int count=0;
    kubelki.resize(maxcost+1);
    d[source]=0;
    kubelki[0].insert(source);

    for (int i = 0; count<n; i=(i+1)%(maxcost+1)) {
        while (!kubelki[i].empty()) {
            int u = *kubelki[i].begin();
            kubelki[i].erase(kubelki[i].begin());
            count++;
            
            if (i > d[u]) continue;
            
            for (auto& it : graph[u]) {
                int v = std::get<0>(it);
                int weight = std::get<1>(it);
                int newDist = d[u] + weight;
                
                if (newDist < d[v]) {
                    
                    if (d[v] != LONG_MAX) {
                        kubelki[(d[v])%(maxcost+1)].erase(v);
                    }
                    
                    d[v] = newDist;
                    kubelki[newDist%(maxcost+1)].insert(v);
                }
            }
        }
    }
    
    auto t1 = std::chrono::high_resolution_clock::now();
    double elapsed_seconds = std::chrono::duration<double>(t1 - t0).count();
    //std::cout << "Diala elapsed: " << elapsed_seconds << " s" << std::endl;
    return elapsed_seconds;
}

long DialaSearch(std::vector<std::vector<std::pair<int,int>>> & graph,int source,int maxcost,int dest,int n){
    std::vector<long> d(graph.size(),LONG_MAX);
    std::vector<std::unordered_set<int>> kubelki;
    int count=1;
    kubelki.resize(maxcost+1);
    d[source]=0;
    kubelki[0].insert(source);

    for (int i = 0; count<n; i=(i+1)%(maxcost+1)) {
        while (!kubelki[i].empty()) {
            int u = *kubelki[i].begin();
            kubelki[i].erase(kubelki[i].begin());
            if(u==dest)return d[u];
            
            if (i > d[u]) continue;
            
            for (auto& it : graph[u]) {
                int v = std::get<0>(it);
                int weight = std::get<1>(it);
                int newDist = d[u] + weight;
                
                if (newDist < d[v]) {
                    
                    if (d[v] != LONG_MAX) {
                        kubelki[(d[v])%(maxcost+1)].erase(v);
                    }
                    
                    d[v] = newDist;
                    kubelki[newDist%(maxcost+1)].insert(v);
                }
            }
        }
    }
    
    return -1;
}

long OGDialaSearch(std::vector<std::vector<std::pair<int,int>>> & graph,int source,int maxcost,int dest){
    std::vector<long> d(graph.size(),LONG_MAX);
    std::vector<std::unordered_set<int>> kubelki;
    kubelki.resize(graph.size()*maxcost+1);
    d[source]=0;
    kubelki[0].insert(source);

    for (int i = 0; i < kubelki.size(); i++) {
        while (!kubelki[i].empty()) {
            int u = *kubelki[i].begin();
            kubelki[i].erase(kubelki[i].begin());
            //if(u==dest) return d[dest];
            if (i > d[u]) continue;
            
            for (auto& it : graph[u]) {
                int v = std::get<0>(it);
                int weight = std::get<1>(it);
                int newDist = d[u] + weight;
                
                if (newDist < d[v]) {
                    
                    if (d[v] != LONG_MAX) {
                        kubelki[d[v]].erase(v);
                    }
                    
                    d[v] = newDist;
                    kubelki[newDist].insert(v);
                }
            }
        }
    }
    
    return d[dest];
}
