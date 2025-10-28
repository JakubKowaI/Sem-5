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

void BFS(vector<Node>& N){
    if (N.empty()) return;
    N[0].marked = true;
    N[0].pred = nullptr;
    int next = 1;
    N[0].order = next;
    queue<Node*> LIST;
    LIST.push(&N[0]);
    cout<<"Node: "<<N[0].label<<" order: "<<N[0].order<<endl;
    while(LIST.empty()!=1){
        Node* i=LIST.front();
        bool incident=0;        
        for(auto j : i->edges){
            if(j->marked==0&&!incident){
                j->marked=1;
                j->pred=i;
                next++;
                j->order=next;
                cout<<"Node: "<<j->label<<" order: "<<j->order<<endl;
                LIST.push(j);
                incident=1;
            }
        }
        if(!incident)LIST.pop();
    }
}

void DFS(vector<Node>& N){
    if (N.empty()) return;
    N[0].marked = 1;
    N[0].pred = nullptr;
    int next = 1;
    N[0].order = next;
    stack<Node*> LIST;
    LIST.push(&N[0]);
    cout<<"Node: "<<N[0].label<<" order: "<<N[0].order<<endl;
    while(LIST.empty()!=1){
        Node* i=LIST.top();
        bool incident=0;        
        for(auto j : i->edges){
            if(j->marked==0&&!incident){
                j->marked=1;
                j->pred=i;
                next++;
                j->order=next;
                cout<<"Node: "<<j->label<<" order: "<<j->order<<endl;
                LIST.push(j);
                incident=1;
            }
        }
        if(!incident)LIST.pop();
    }
}

// void printVisit(Node* root){
//     int order=1;
//     for(auto n : root->edges){

//     }
// }

void printTree(Node* n){
    if (!n) return;
    for(auto e : n->edges){
        if(e->pred==n){
            cout<<"Edge: "<<n->label<<" - "<<e->label<<endl;
            printTree(e);
        }
    }
}

int main(int argc, char** argv) {
    bool treeMode = false;
    if(argc!=3){
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
    if(string(argv[2])=="--tree"){
        cout<<"TreeMode activated"<<endl;
        treeMode=true;
    }

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
    vector<Node> B=buildGraph(n,m,edges,directed);
    BFS(N);
    if(treeMode){
        printTree(&N[0]);
    }
    cout<< endl;
    DFS(B);
    if(treeMode){
        printTree(&B[0]);
    }
    
    return 0;
}