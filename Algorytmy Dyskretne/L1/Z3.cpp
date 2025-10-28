#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <fstream>

using namespace std;

struct Node{
    vector<Node*> edges;
    int label;
    Node* pred=nullptr;
    int order;
    bool marked=0;

    Node(int label,Node* pred,int order,bool marked){
        this->label=label;
        this->order=order;
        this->pred=pred;
        this->marked=marked;
    }
    Node(int label){
        this->label=label;
        this->pred = nullptr;
        this->order = 0;
        this->marked = false;
    }
};

Node* findNode(int label,vector<Node*> nodes){
    for (Node* n : nodes) if (n->label == label) return n;
        return nullptr;
}

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

vector<Node> reverseGraph(vector<Node>& G, vector<pair<int,int>> edges) {
    int n = G.size();
    vector<Node> GT;
    GT.reserve(n);
    for (int i = 0; i < n; i++) GT.emplace_back(G[i].label);

    for (auto [u, v] : edges) {
        GT[v - 1].edges.push_back(&GT[u - 1]);
    }
    return GT;
}

vector<Node*> DFS(vector<Node>& N){
    vector<Node*> component;
    if (N.empty()) return component;
    stack<Node*> LIST;
    int next = 1;
    for(int i=0;i<N.size();i++){
        if(!N[i].marked){
            N[i].marked = 1;
            N[i].pred = nullptr;
            N[i].order = next;
            LIST.push(&N[i]);
            component.push_back(&N[i]);
            break;
        }
    }
    
    //cout<<"Node: "<<N[0].label<<" order: "<<N[0].order<<endl;
    while(LIST.empty()!=1){
        Node* i=LIST.top();
        bool incident=0;        
        for(auto j : i->edges){
            if(j->marked==0&&!incident){
                j->marked=1;
                j->pred=i;
                next++;
                j->order=next;
                //cout<<"Node: "<<j->label<<" order: "<<j->order<<endl;
                LIST.push(j);
                component.push_back(j);
                incident=1;
            }
        }
        if(!incident)LIST.pop();
    }
    return component;
}

vector<Node*> DFSFromNode(Node* n){
    vector<Node*> component;
    if (n==nullptr) return component;
    stack<Node*> LIST;
    int next = 1;
    
            n->marked = 1;
            n->order = next;
            LIST.push(n);
            component.push_back(n);
         
    //cout<<"Node: "<<N[0].label<<" order: "<<N[0].order<<endl;
    while(LIST.empty()!=1){
        Node* i=LIST.top();
        bool incident=0;        
        for(auto j : i->edges){
            if(j->marked==0&&!incident){
                j->marked=1;
                j->pred=i;
                next++;
                j->order=next;
                //cout<<"Node: "<<j->label<<" order: "<<j->order<<endl;
                LIST.push(j);
                component.push_back(j);
                incident=1;
            }
        }
        
        if(!incident)LIST.pop();
        next++;
        i->order=next;
    }
    return component;
}

void stronglyConnected(vector<Node>& N,vector<pair<int,int>> edges){
    DFS(N);

    vector<Node> GT=reverseGraph(N,edges);
    vector<vector<Node*>> components;
    for(auto it:GT){
        //cout<<it.label<<endl;
        if(it.marked)continue;
        vector<Node*> tempComponent=DFSFromNode(&it);
        if(!tempComponent.empty())components.push_back(tempComponent);
    }
    int i=1;
    for(auto itC:components){
        cout<<"Component: "<<i<<endl;
        i++;
        for(auto node:itC){
            cout<<node->label<<endl;
        }
    }
}

int main(int argc, char** argv) {
    bool treeMode = false;
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
    vector<Node> G=buildGraph(n,m,edges,directed);
    stronglyConnected(G,edges);
    
    return 0;
}