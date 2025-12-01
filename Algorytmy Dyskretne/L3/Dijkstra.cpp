#include "Algorithms.hpp"

std::pair<int,int> Dijsktra(std::vector<std::vector<int>> graph,int s){
    MinHeap H(graph.size());
    std::vector<long> d(graph.size(),LONG_MAX);
    d[s]=0;
    std::vector<long> pred(graph.size(),0);
    H.insert(s,0);
    while(!H.isEmpty()){
        HeapNode temp = H.extractMin();
        for(int i=1;i<graph[temp.vertex].size();i++){
            if(graph[temp.vertex][i]>=0){
                long value=d[temp.vertex]+graph[temp.vertex][i];
                if(d[i]>value){
                    if(d[i]==LONG_MAX){
                        d[i]=value;
                        pred[i]=temp.vertex;
                        H.insert(i,d[i]);
                    }else{
                        d[i]=value;
                        pred[i]=temp.vertex;
                        H.decreaseKey(temp.vertex,value);
                    }
                }
            }
        }
    }
    int test=0;
    for(auto it:d){
        std::cout<<test<<" : "<<it<<std::endl;
        test++;
    }
    test=0;
    for(auto it:pred){
        std::cout<<test<<" : "<<it<<std::endl;
        test++;
    }
    return std::pair<int,int>(1,1);
}