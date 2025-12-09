#pragma once
#include <iostream>
#include <utility>
#include <vector>
#include <climits>
#include <chrono>


void swap(int *x, int *y);

struct HeapNode { 
    int vertex; 
    long dist = LONG_MAX; 
};

class MinHeap{
    std::vector<HeapNode> heap;  // przechowuje kopiec
    std::vector<int> pos;        // pos[v] = index w kopcu dla wierzchołka v
    int size;

    public:
    MinHeap(int capacity);
    void swapNodes(int i, int j);
    void MinHeapify(int i);
    int parent(int i);
    int left(int i);
    int right(int i);
    bool isEmpty();
    void heapifyDown(int i);
    void heapifyUp(int i);
    void insert(int vertex, int dist);
    HeapNode extractMin();
    void decreaseKey(int vertex, int newdist);
    void deleteKey(int i);
};


std::vector<long> Dijsktra(std::vector<std::vector<std::pair<int,int>>> &graph, int s,int dest=-1);
//long DijsktraSearch(std::vector<std::vector<std::pair<int,int>>> &graph, int s,int dest);

std::vector<long> Diala(std::vector<std::vector<std::pair<int,int>>> & graph,int source,int maxcost,int n,int dest=-1);
// long DialaSearch(std::vector<std::vector<std::pair<int,int>>> & graph,int source,int maxcost,int dest,int n);

std::vector<long> RadixHeap(std::vector<std::vector<std::pair<int,int>>> & graph,int s,int maxcost,int n,int dest=-1);