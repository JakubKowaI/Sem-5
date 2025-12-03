#include "Algorithms.hpp"

double Dijsktra(std::vector<std::vector<std::pair<int,int>>> &graph, int s){
    auto t0 = std::chrono::high_resolution_clock::now();

    int n = (int)graph.size();
    std::vector<long> d(n, LONG_MAX);
    std::vector<int> pred(n, -1);

    d[s] = 0;
    pred[s]=0;
    MinHeap H(n);
    H.insert(s,0);

    while(!H.isEmpty()){
        int i = H.extractMin().vertex;
        //std::cout<<"Zdejmuje: "<<u<<std::endl;
        if (i == -1) break;

        for (auto it:graph[i]) {
            long value = d[i] + std::get<1>(it);
            int j=std::get<0>(it);

            if(d[j]>value){
                if(d[j]==LONG_MAX){
                    d[j]=value;
                    pred[j]=i;
                    H.insert(j,value);
                }else{
                    d[j]=value;
                    pred[j]=i;
                    H.decreaseKey(j,value);
                }
            }
        }
    }

    // for (int i = 0; i < n; ++i) {
    //     std::cout << i << " : " << d[i] << std::endl;
    // }
    // for (int i = 0; i < n; ++i) {
    //     std::cout << i << " : " << pred[i] << std::endl;
    // }

    auto t1 = std::chrono::high_resolution_clock::now();
    double elapsed_seconds = std::chrono::duration<double>(t1 - t0).count();
    //std::cout << "Dijkstra elapsed: " << elapsed_seconds << " s" << std::endl;
    return elapsed_seconds;
}

long DijsktraSearch(std::vector<std::vector<std::pair<int,int>>> &graph, int s,int dest){    
    int n = (int)graph.size();
    std::vector<long> d(n, LONG_MAX);
    std::vector<int> pred(n, -1);

    d[s] = 0;
    pred[s]=0;
    MinHeap H(n);
    H.insert(s,0);

    while(!H.isEmpty()){
        int i = H.extractMin().vertex;
        //std::cout<<"Zdejmuje: "<<u<<std::endl;
        if (i==dest)return d[dest];
        if (i == -1) break;

        for (auto it:graph[i]) {
            long value = d[i] + std::get<1>(it);
            int j=std::get<0>(it);

            if(d[j]>value){
                if(d[j]==LONG_MAX){
                    d[j]=value;
                    pred[j]=i;
                    H.insert(j,value);
                }else{
                    d[j]=value;
                    pred[j]=i;
                    H.decreaseKey(j,value);
                }
            }
        }
    }
    return -1;
}