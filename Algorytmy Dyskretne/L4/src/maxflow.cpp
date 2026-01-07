#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <cmath>
#include <algorithm>
#include <chrono>
#include "flow_utils.hpp"

using namespace std;

int hammingWeight(int n) {
    int count = 0;
    while (n > 0) {
        n &= (n - 1);
        count++;
    }
    return count;
}

int zeroCount(int n, int k) {
    return k - hammingWeight(n);
}

int main(int argc, char* argv[]) {
    int k = -1;
    bool printFlow = false;
    string algorithm = "ek"; // ek || dinic
    string glpkFile = "";

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--size") {
            if (i + 1 < argc) k = stoi(argv[++i]);
        } else if (arg == "--printFlow") {
            printFlow = true;
        } else if (arg == "--algorithm") {
             if (i + 1 < argc) algorithm = argv[++i];
        } else if (arg == "--glpk") {
             if (i + 1 < argc) glpkFile = argv[++i];
        }
    }

    if (k < 1) {
        cerr << "Usage: " << argv[0] << " --size k [--printFlow] [--algorithm ek|dinic] [--glpk filename]" << endl;
        return 1;
    }

    int numVertices = 1 << k;
    int source = 0;
    int sink = numVertices - 1;

    FlowNetwork graph(numVertices);
    
    mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());

    for (int u = 0; u < numVertices; ++u) {
        int hu = hammingWeight(u);
        int zu = zeroCount(u, k);

        for (int bit = 0; bit < k; ++bit) {
            if (!((u >> bit) & 1)) {
                int v = u | (1 << bit);
             
                int hv = hammingWeight(v);
                int zv = zeroCount(v, k);
                
                int l = max({hu, zu, hv, zv});
                int maxCap = 1 << l;
                
                uniform_int_distribution<int> dist(1, maxCap);
                int cap = dist(rng);
                
                graph.addEdge(u, v, cap);
            }
        }
    }

    if (!glpkFile.empty()) {
        graph.writeToLP(glpkFile, source, sink);
    }

    auto start = chrono::high_resolution_clock::now();
    long long flow = 0;

    if (algorithm == "dinic") {
        flow = graph.dinic(source, sink);
    } else {
        flow = graph.edmondsKarp(source, sink);
    }
    
    auto end = chrono::high_resolution_clock::now();
    auto elapsed_ms = chrono::duration_cast<chrono::microseconds>(end - start).count();

    cout << flow << endl;
    cerr << "Time: " << elapsed_ms << "s" << endl;
    cerr << "AugmentingPaths: " << graph.augmentingPaths << endl;

    if (printFlow) {
        for (int u = 0; u < numVertices; ++u) {
            for (const auto& e : graph.adj[u]) {
                if (e.capacity > 0) {
                     cout << u << " -> " << e.to << " flow: " << e.flow << " / " << e.capacity << endl;
                }
            }
        }
    }

    return 0;
}
