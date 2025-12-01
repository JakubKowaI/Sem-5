#pragma once
#include <iostream>
#include <bits/stdc++.h>
#include <utility>
#include <vector>
#include <climits>

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

std::pair<int,int> Dijsktra(std::vector<std::vector<int>> graph,int s);