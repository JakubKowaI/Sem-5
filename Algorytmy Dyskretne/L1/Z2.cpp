#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <queue>
#include <stack>

using namespace std;

struct Node{
int label;
Node* pred = nullptr;
vector<Node*> edges;
int order;
int indegree=0;
Node(int label){
    this->label=label;
}
};

vector<Node> buildGraph(int n,int m,vector<pair<int,int>> edges,bool directed){
    vector<Node> N;
    N.reserve(n);
    for(int i=0;i<n;i++){
        N.emplace_back(i+1);
    }
    for(auto e : edges){
        int l = get<0>(e);
        int p = get<1>(e);
        
        if(l < 1 || l > n || p < 1 || p > n) continue;
        if(find(N[l-1].edges.begin(),N[l-1].edges.end(),&N[p-1])==N[l-1].edges.end()){
            N[l-1].edges.push_back(&N[p-1]);
        }
        if(!directed){
            if(find(N[p-1].edges.begin(),N[p-1].edges.end(),&N[l-1])==N[p-1].edges.end()){
                N[p-1].edges.push_back(&N[l-1]);
            }
        }
    }
    return N;
}

void topologicalSort(vector<Node>& N, const vector<pair<int,int>>& edges){
    for(size_t i=0;i<N.size();++i){
        N[i].indegree = 0;
        N[i].order = 0;
    }

    for(const auto &e : edges){
        int to = get<1>(e);
        if(to >= 1 && to <= (int)N.size()){
            N[to-1].indegree++;
        }
    }

    queue<Node*> q;
    int next = 0;
    for(int i=0;i<(int)N.size();++i){
        if(N[i].indegree==0) q.push(&N[i]);
    }

    while(!q.empty()){
        Node* v = q.front(); q.pop();
        v->order = ++next;
        for(auto neighbor : v->edges){
            if(--(neighbor->indegree) == 0){
                q.push(neighbor);
            }
        }
    }

    if(next < (int)N.size()){
        cout<<"Jest cykl w grafie"<<endl;
    } else {
        cout<<"Nie ma cykli w grafie"<<endl;
        if(N.size()>200)return;

        cout << "Topologiczne porzadek (label : order):\n";
        for(const auto &node : N) cout << node.label << " : " << node.order << "\n";
    }
}

int main(int argc, char** argv){
    if(argc!=2){
        cerr<<"Wrong argument number!"<<endl;
        return -1;
    }
    ifstream input(argv[1]);
    bool directed=true;
    string firstLine;
    input>>firstLine;
    if(firstLine!="D"&&firstLine!="U"){
        cerr<<"Problem z danymi"<<endl;
    }
    if(firstLine=="U")directed=false;

    vector<pair<int,int>> edges;
    int n,m;
    try{
        input>>firstLine;
        n=stoi(firstLine);
        input>>firstLine;
        m=stoi(firstLine);
        string temp,temp2;
        while (input>> temp>>temp2)
        {
            edges.push_back(pair<int,int>(stoi(temp),stoi(temp2)));
        }
    }catch(exception e){
        cerr<<e.what()<<endl;
    }
    
    vector<Node> N=buildGraph(n,m,edges,directed);
    cout<<"Done"<<endl;
    topologicalSort(N,edges);

    return 0;
}