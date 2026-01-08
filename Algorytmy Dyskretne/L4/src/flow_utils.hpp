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

    void addEdge(int from, int to, int cap) {
        Edge a = {to, cap, 0, (int)adj[to].size()};
        Edge b = {from, 0, 0, (int)adj[from].size()};
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

            //BFS
            while (!q.empty()) {
                int u = q.front().first;
                int flow = q.front().second;
                q.pop();

                if (u == t) {
                    pathFlow = flow;
                    lastNode = t;
                    break;
                }

                for (int v = 0; v < adj[u].size(); ++v) {
                    Edge &e = adj[u][v];
                    if (parent[e.to] == -1 && e.capacity - e.flow > 0) {
                        parent[e.to] = u;
                        parentEdgeIndex[e.to] = v;
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


    void writeToLP(const string& filename) {
        ofstream out(filename);
        out << "using JuMP\n";
        out << "using GLPK\n";
        out << "model = Model(GLPK.Optimizer)\n";
        out << "set_silent(model)\n";
        out << "G = zeros(Int, "<<n<<", "<<n<<")\n";

        int m = 0;
        for(int u=0; u<n; ++u) {
            for(auto& e : adj[u]) {
                if (e.capacity > 0){
                    m++;
                    out << "G["<<u+1<<","<<e.to+1<<"]="<<e.capacity<<"\n";
                } 
            }
        }

        out << "# Nodes: " << n << ", Edges: " << m << "\n";
        out << "@variable(model, f[1:"<<n<<", 1:"<<n<<"] >= 0)\n";        
        out << "@constraint(model, [i = 1:"<<n<<", j = 1:"<<n<<"], f[i, j] <= G[i, j])\n";
        out << "@constraint(model, [i = 1:"<<n<<"; i != 1 && i != "<<n<<"], sum(f[i, :]) == sum(f[:, i]))\n";
        out << "@objective(model, Max, sum(f[1, :]))\n";
        out << "optimize!(model)\n";
        out << "println(objective_value(model))\n";
        out.close();
    }
};

#endif
