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
                // if (d[std::get<0>(it)] == LONG_MAX) {
                //     d[std::get<0>(it)] = nd;
                //     pred[std::get<0>(it)] = i;
                //     H.insert(std::get<0>(it));
                // } else if (nd < d[std::get<0>(it)]) {
                //     d[std::get<0>(it)] = nd;
                //     pred[std::get<0>(it)] = i;
                //     if (H.contains(std::get<0>(it))) H.decreaseKey(std::get<0>(it));
                //     else H.insert(std::get<0>(it));
                // }
                if(d[std::get<0>(it)]>value){
                    if(d[std::get<0>(it)]==LONG_MAX){
                        d[std::get<0>(it)]=value;
                        pred[std::get<0>(it)]=i;
                        H.insert(std::get<0>(it),value);
                    }else{
                        d[std::get<0>(it)]=value;
                        pred[std::get<0>(it)]=i;
                        H.decreaseKey(i,value);
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

long DijsktraSearch(std::vector<std::vector<std::pair<int,int>>> graph, int s,int dest){    
    int n = (int)graph.size();
    std::vector<long> d(n, LONG_MAX);
    std::vector<int> pred(n, -1);

    // d[s] = 0;
    // MinHeap H(n, d);
    // H.insert(s);

    // while(!H.isEmpty()){
    //     int u = H.extractMin();
    //     if (u==dest) break;
    //     if (u == -1) break;

    //     for (auto it:graph[u]) {
    //             long nd = d[u] + std::get<1>(it);
    //             if (d[std::get<0>(it)] == LONG_MAX) {
    //                 d[std::get<0>(it)] = nd;
    //                 pred[std::get<0>(it)] = u;
    //                 H.insert(std::get<0>(it));
    //             } else if (nd < d[std::get<0>(it)]) {
    //                 d[std::get<0>(it)] = nd;
    //                 pred[std::get<0>(it)] = u;
    //                 if (H.contains(std::get<0>(it))) H.decreaseKey(std::get<0>(it));
    //                 else H.insert(std::get<0>(it));
    //             }
            
    //     }
    // }

    // for (int i = 0; i < n; ++i) {
    //     std::cout << i << " : " << d[i] << std::endl;
    // }
    // for (int i = 0; i < n; ++i) {
    //     std::cout << i << " : " << pred[i] << std::endl;
    // }

    return d[dest];
}