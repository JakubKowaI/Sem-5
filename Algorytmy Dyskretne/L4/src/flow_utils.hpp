#ifndef FLOW_UTILS_HPP
#define FLOW_UTILS_HPP

#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct Edge {
    int to;
    int capacity;
    int flow;
    int rev; // index of reverse edge in adj[to]
    int id; // original index for tracking if needed
};

class FlowNetwork {
public:
    int n;
    vector<vector<Edge>> adj;
    
    // For Dinic
    vector<int> level;
    vector<int> ptr;

    long long augmentingPaths = 0;

    FlowNetwork(int n) : n(n), adj(n), level(n), ptr(n) {}

    void addEdge(int from, int to, int cap, int id = -1) {
        Edge a = {to, cap, 0, (int)adj[to].size(), id};
        Edge b = {from, 0, 0, (int)adj[from].size(), -1}; // Residual edge has 0 capacity initially for directed graph
        adj[from].push_back(a);
        adj[to].push_back(b);
    }

    long long edmondsKarp(int s, int t) {
        long long maxFlow = 0;
        augmentingPaths = 0;
        
        while (true) {
            vector<int> parent(n, -1);
            vector<int> parentEdgeIndex(n, -1);
            queue<pair<int, int>> q;
            
            q.push({s, numeric_limits<int>::max()});
            parent[s] = s; 

            int pathFlow = 0;
            int lastNode = -1;

            while (!q.empty()) {
                int u = q.front().first;
                int flow = q.front().second;
                q.pop();

                if (u == t) {
                    pathFlow = flow;
                    lastNode = t;
                    break;
                }

                for (int i = 0; i < adj[u].size(); ++i) {
                    Edge &e = adj[u][i];
                    if (parent[e.to] == -1 && e.capacity - e.flow > 0) {
                        parent[e.to] = u;
                        parentEdgeIndex[e.to] = i;
                        int newFlow = min(flow, e.capacity - e.flow);
                        q.push({e.to, newFlow});
                    }
                }
                if (lastNode != -1) break;
            }

            if (lastNode == -1) break; // No augmenting path

            augmentingPaths++;
            maxFlow += pathFlow;
            
            int curr = t;
            while (curr != s) {
                int prev = parent[curr];
                int idx = parentEdgeIndex[curr];
                
                adj[prev][idx].flow += pathFlow;
                int revIdx = adj[prev][idx].rev;
                adj[curr][revIdx].flow -= pathFlow;
                
                curr = prev;
            }
        }
        return maxFlow;
    }

    // Dinic's Algorithm Helpers
    bool bfsEstimate(int s, int t) {
        fill(level.begin(), level.end(), -1);
        level[s] = 0;
        queue<int> q;
        q.push(s);
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (const auto& e : adj[u]) {
                if (e.capacity - e.flow > 0 && level[e.to] == -1) {
                    level[e.to] = level[u] + 1;
                    q.push(e.to);
                }
            }
        }
        return level[t] != -1;
    }

    int dfsPush(int u, int t, int pushed) {
        if (pushed == 0) return 0;
        if (u == t) return pushed;
        for (int& i = ptr[u]; i < adj[u].size(); ++i) {
            Edge &e = adj[u][i];
            int tr = e.to;
            if (level[u] + 1 != level[tr] || e.capacity - e.flow == 0) continue;
            int push = dfsPush(tr, t, min(pushed, e.capacity - e.flow));
            if (push == 0) continue;
            e.flow += push;
            adj[tr][e.rev].flow -= push; // Residual
            return push;
        }
        return 0;
    }

    // Dinic
    long long dinic(int s, int t) {
        long long maxFlow = 0;
        augmentingPaths = 0; // In Dinic, we can count phases or paths. Standard EK metric is paths. 
                             // Counting exact "augmenting paths" in Dinic is tricky because it pushes multiple paths in one DFS.
                             // We will count the number of DFS pushes that result in flow > 0.
        
        while (bfsEstimate(s, t)) {
            fill(ptr.begin(), ptr.end(), 0);
            while (int pushed = dfsPush(s, t, numeric_limits<int>::max())) {
                maxFlow += pushed;
                augmentingPaths++; 
            }
        }
        return maxFlow;
    }

    void writeToLP(const string& filename, int s, int t) {
        ofstream out(filename);
        // We will generate a Julia file using JuMP
        out << "using JuMP\n";
        out << "using GLPK\n";
        out << "model = Model(GLPK.Optimizer)\n";
        out << "set_silent(model)\n"; // Suppress solver output

        
        // Count edges (forward only)
        int m = 0;
        for(int u=0; u<n; ++u) {
            for(auto& e : adj[u]) {
                if (e.capacity > 0) m++; // Only original edges or those with capacity
            }
        }

        out << "# Nodes: " << n << ", Edges: " << m << "\n";
        
        // Variables: flow for each edge
        // Map (u, v) -> variable index? Or just use strings/symbols
        // JuMP allows array of variables.
        
        // Let's create a list of edges to index them
        out << "flows = @variable(model, f[1:" << m << "] >= 0)\n";
        
        int edgeParamsIdx = 1;
        
        // Store flow conservation constraints strings
        vector<string> flow_conservation(n, "0");
        
        for(int u=0; u<n; ++u) {
            for(auto& e : adj[u]) {
                if (e.capacity > 0) { // Forward edge
                    out << "set_upper_bound(f[" << edgeParamsIdx << "], " << e.capacity << ")\n";
                    
                    // Outgoing from u
                    // flow_conservation[u] -= f[i]
                    flow_conservation[u] += " - f[" + to_string(edgeParamsIdx) + "]";
                    
                    // Incoming to e.to
                    // flow_conservation[e.to] += f[i]
                    flow_conservation[e.to] += " + f[" + to_string(edgeParamsIdx) + "]";
                    
                    edgeParamsIdx++;
                }
            }
        }

        // Constraints
        for(int i=0; i<n; ++i) {
            if (i == s || i == t) continue;
            out << "@constraint(model, " << flow_conservation[i] << " == 0)\n";
        }
        
        // Objective: max sum of outgoing from s (or incoming to t)
        string obj = "0";
        // Iterate edges from s
        edgeParamsIdx = 1; 
         for(int u=0; u<n; ++u) {
            for(auto& e : adj[u]) {
                if(e.capacity > 0) {
                     if (u == s) {
                        obj += " + f[" + to_string(edgeParamsIdx) + "]";
                     }
                     edgeParamsIdx++;
                }
            }
        }
        
        out << "@objective(model, Max, " << obj << ")\n";
        out << "optimize!(model)\n";
        out << "println(objective_value(model))\n";
        out.close();
    }
};

#endif
