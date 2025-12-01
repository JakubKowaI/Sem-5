#include "Algorithms.hpp"

using namespace std;

// Prototype of a utility function to swap two integers
void swap(int *x, int *y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

// struct HeapNode { 
//     int vertex; 
//     int dist; 
// };




    MinHeap::MinHeap(int n) {
        heap.resize(n);
        pos.resize(n);
        size = 0;
    }

    // Zamienia dwa elementy + aktualizuje pos[]
    void MinHeap::swapNodes(int i, int j) {
        HeapNode temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;

        pos[heap[i].vertex] = i;
        pos[heap[j].vertex] = j;
    }

    int MinHeap::parent(int i) { return (i - 1) / 2; }
    int MinHeap::left(int i)   { return 2 * i + 1; }
    int MinHeap::right(int i)  { return 2 * i + 2; }

    bool MinHeap::isEmpty() { return size == 0; }

    // Naprawia kopiec w dół
    void MinHeap::heapifyDown(int i) {
        int l = left(i);
        int r = right(i);
        int smallest = i;

        if (l < size && heap[l].dist < heap[smallest].dist)
            smallest = l;

        if (r < size && heap[r].dist < heap[smallest].dist)
            smallest = r;

        if (smallest != i) {
            swapNodes(i, smallest);
            heapifyDown(smallest);
        }
    }

    // Naprawia kopiec w górę
    void MinHeap::heapifyUp(int i) {
        while (i > 0 && heap[i].dist < heap[parent(i)].dist) {
            swapNodes(i, parent(i));
            i = parent(i);
        }
    }

    // Wstawia nowy wierzchołek (vertex, dist)
    void MinHeap::insert(int vertex, int dist) {
        heap[size].vertex = vertex;
        heap[size].dist   = dist;
        pos[vertex] = size;
        size++;
        heapifyUp(size - 1);
    }

    // Zwraca i usuwa korzeń (najmniejszy dystans)
    HeapNode MinHeap::extractMin() {
        if (size == 0) return { -1, -1 };

        HeapNode root = heap[0];

        heap[0] = heap[size - 1];
        pos[heap[0].vertex] = 0;

        size--;
        heapifyDown(0);

        return root;
    }

    // decrease-key: zmniejsza dystans dla konkretnego wierzchołka
    void MinHeap::decreaseKey(int vertex, int newDist) {
        int i = pos[vertex];
        heap[i].dist = newDist;
        heapifyUp(i);
    }