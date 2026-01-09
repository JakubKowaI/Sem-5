#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <set>
#include "flow_utils.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    int k = -1;
    int degree = -1;
    bool printMatching = false;
    string glpkFile = "";

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--size") {
            if (i + 1 < argc) k = stoi(argv[++i]);
        } else if (arg == "--degree") {
            if (i + 1 < argc) degree = stoi(argv[++i]);
        } else if (arg == "--printMatching") {
            printMatching = true;
        } else if (arg == "--glpk") {
             if (i + 1 < argc) glpkFile = argv[++i];
        }
    }

    if (k < 1 || degree < 1 || degree > k) {
        if (k < 1) {
             cerr << "Usage: " << argv[0] << " --size k --degree i [--printMatching] [--glpk filename]" << endl;
             return 1;
        }
    }

    int sizeV = 1 << k;
    int numVertices = 2 * sizeV + 2; // source + V1 + V2 + sink
    int source = 0;
    int sink = numVertices - 1;
    // V1 1 : sizeV
    // V2 sizeV + 1 : 2*sizeV

    FlowNetwork graph(numVertices);
    mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> v2Dist(0, sizeV - 1);

    for (int u = 0; u < sizeV; ++u) {
        graph.addEdge(source, u + 1, 1);
    }

    for (int v = 0; v < sizeV; ++v) {
        graph.addEdge(sizeV + 1 + v, sink, 1);
    }

    for (int u = 0; u < sizeV; ++u) {
        for (int d = 0; d < degree; ++d) {
            int v = v2Dist(rng);
            
            graph.addEdge(u + 1, sizeV + 1 + v, 1);
        }
    }

    if (!glpkFile.empty()) {
        graph.writeToLP(glpkFile);
    }

    auto start = chrono::high_resolution_clock::now();
    long long matchSize = graph.edmondsKarp(source, sink); 
    
    auto end = chrono::high_resolution_clock::now();
    auto elapsed_ms = chrono::duration_cast<chrono::microseconds>(end - start).count();

    cout << matchSize << endl;
    cerr << "Time: " << elapsed_ms << "s" << endl;

    if (printMatching) {
        for (int u = 0; u < sizeV; ++u) {
            int uNode = u + 1;
            for (const auto& e : graph.adj[uNode]) {
                if (e.to > sizeV && e.to < sink && e.flow == 1) {
                    cout << u << " -- " << (e.to - sizeV - 1) << endl;
                }
            }
        }
    }

    return 0;
}
