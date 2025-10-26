#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <queue>

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
    N.reserve(n); // avoid reallocation so addresses &N[i] remain stable
    for(int i=0;i<n;i++){
        N.emplace_back(i+1);
    }
    for(auto e : edges){
        int l = get<0>(e);
        int p = get<1>(e); // was get<0>(e) previously (bug)
        // validate 1-based labels
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

void topologicalSort(vector<Node> N,vector<pair<int,int>> edges){
    // for(int i=0;i<N.size();i++){
    //     N[i].indegree=0;
    // }
    // compute indegrees from edge list (assumes edges are 1-based labels)
    for(size_t i=0;i<edges.size();i++){
        int temp = get<1>(edges[i]);
        if(temp >= 1 && temp <= (int)N.size()){
            N[temp-1].indegree++;
        }
    }
    queue<Node*> LIST;
    int next=0;
    for(int i=0;i<N.size();i++){
        if(N[i].indegree==0)LIST.push(&N[i]);
    }
    while (!LIST.empty()){
        Node* i = LIST.front();
        LIST.pop();
        next++;
        i->order = next;
        for(auto it = i->edges.begin(); it != i->edges.end(); ++it){
            // *it is Node* -> access indegree via (*it)->indegree
            if(--((*it)->indegree) == 0){
                LIST.push(*it);
            }
        }
    }
    if(next<N.size()){
        cout<<"Jest cykl w grafie"<<endl;
    }else{
        cout<<"Nie ma cykli w grafie"<<endl;
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