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

    // Argument parsing
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
        // Warning: Problem statement says i <= k. 
        // If i can be larger than k but task says i <= k in the param text. "k \in {1..16} oraz i <= k"
        // Wait, "degree i neighbors".
        // If the task meant i distinct neighbors, I must sample without replacement.
        // It says "i neighbors selected independently and uniformly at random".
        // Independently + Uniformly usually implies with replacement allowed?
        // "wybranych niezależnie i jednostajnie losowo" - if they are independent, it's with replacement.
        // If I pick the same neighbor twice, is it a multigraph?
        // "dwudzielny graf losowy... krawędzie generowane są w taki sposób... ma i sąsiadów"
        // Usually matching is on simple graphs.
        // If I select "independent and uniformly", I might pick the same vertex multiple times.
        // Interpretation A: Simple graph, select i distinct neighbors.
        // Interpretation B: Multigraph allowed, capacity > 1? No, matching is 1-to-1.
        // Interpretation C: We drop duplicates.
        // Given "i neighbors", usually means degree is i.
        // But "independent" strongly suggests replacement.
        // I will assume simple graph is required for matching, so I will treat duplicate picks as "same edge", effectively resulting in degree <= i in the multigraph sense,
        // OR I should use a set to ensure exactly i distinct neighbors? 
        // If I pick independently, I might get fewer than i distinct neighbors.
        // Let's re-read carefully: "każdy wierzchołek z V1 ma i sąsiadów z V2 wybranych ... losowo".
        // This phrasing usually comes from random graph models where we specify the process (e.g. configuration model or fixed out-degree with replacement).
        // Since we compute MATCHING, parallel edges don't help. 
        // I will implement "pick i times with replacement", and add edge if not exists. 
        // Wait, normally "d-regular random graph" picks d distinct.
        // But "independently" suggests replacement.
        // I'll stick to: Pick i times. Each time uniform from V2. Add edge. (Parallel edges irrelevant for matching size on simple graph, but might matter for flow if cap adds up? Matching is usually defined on simple graph).
        // Let's use simple graph edges (cap 1). Duplicates don't add capacity in standard matching.
        
        if (k < 1) { // Basic check
             cerr << "Usage: " << argv[0] << " --size k --degree i [--printMatching] [--glpk filename]" << endl;
             return 1;
        }
    }

    int sizeV = 1 << k;
    int numVertices = 2 * sizeV + 2; // Source + V1 + V2 + Sink
    int source = 0;
    int sink = numVertices - 1;
    // V1 indices: 1 .. sizeV
    // V2 indices: sizeV + 1 .. 2*sizeV

    FlowNetwork graph(numVertices);
    mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> v2Dist(0, sizeV - 1);

    // Edges S -> V1
    for (int u = 0; u < sizeV; ++u) {
        graph.addEdge(source, u + 1, 1);
    }

    // Edges V2 -> T
    for (int v = 0; v < sizeV; ++v) {
        graph.addEdge(sizeV + 1 + v, sink, 1);
    }

    // Random Edges V1 -> V2
    for (int u = 0; u < sizeV; ++u) {
        // "i neighbors"
        for (int d = 0; d < degree; ++d) {
            int target = v2Dist(rng);
            // Check if edge exists? 
            // In max flow formulation for matching, parallel edges with cap 1 betwen U and V 
            // are equivalent to 1 edge with cap 1 if we only care about if they assume same matching?
            // Actually, if I have 2 edges between u and v, I can use at most 1 in matching.
            // So if I add multiple edges, each with cap 1?
            // "Wielkość skojarzenia" -> Matching involving unique vertices.
            // If I add multiple parallel edges, max flow could theoretically partial flow? No, integer flow.
            // But if I put capacity > 1 between u and v, it doesn't help because S->u has cap 1 and v->T has cap 1.
            // So parallel edges don't hurt, they are redundant.
            // I will just add them as is, FlowNetwork handles multigraphs (adj list).
            // But strict matching definition is set of edges without common vertices.
            
            graph.addEdge(u + 1, sizeV + 1 + target, 1);
        }
    }

    // Generate LP if requested
    if (!glpkFile.empty()) {
        graph.writeToLP(glpkFile, source, sink);
    }

    auto start = chrono::high_resolution_clock::now();
    long long matchSize = graph.edmondsKarp(source, sink); 
    // Usually Hopcroft-Karp is faster, but EK on unit network is O(E * sqrt(V)) which is competitive for sparse graphs.
    // Task 2 doesn't specify algorithm, just "program". I can use EK.
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << matchSize << endl;
    cerr << "Time: " << elapsed.count() << "s" << endl;

    if (printMatching) {
        // Iterate edges from V1 to V2 that have flow 1
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
