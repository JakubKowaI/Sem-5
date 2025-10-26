#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>

using namespace std;

struct Node{
    vector<Node*> edges;
    int label;
    Node* pred;
    int order;
    bool marked;

    Node(int label,Node* pred,int order,bool marked){
        this->label=label;
        this->order=order;
        this->pred=pred;
        this->marked=marked;
    }
};

Node* findNode(int label,vector<Node*> nodes){
    for (Node* n : nodes) if (n->label == label) return n;
        return nullptr;
}

Node* buildGraph(const vector<pair<int,int>>& edges, bool directed) {
    vector<Node*> nodes;

    for (auto t : edges) {
        int a = t.first, b = t.second;
        Node* left = findNode(a,nodes);
        Node* right = findNode(b,nodes);

        if (!left) {
            left = new Node(a, nullptr, -1, 0);
            nodes.push_back(left);
        }

        if (!right) {
            right = new Node(b, nullptr, -1, 0);
            nodes.push_back(right);
        }

        if (find(left->edges.begin(), left->edges.end(), right) == left->edges.end())
            left->edges.push_back(right);

        if (!directed) {
            if (find(right->edges.begin(), right->edges.end(), left) == right->edges.end())
                right->edges.push_back(left);
        }
    }

    if (!nodes.empty()) return nodes[0];
    return nullptr;
}

void BFS(Node* root){
    root->marked=1;
    root->pred=nullptr;
    int next=1;
    root->order=next;
    queue<Node*> LIST;
    LIST.push(root);
    cout<<"Node: "<<root->label<<" order: "<<root->order<<endl;
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

void DFS(Node* root){
    root->marked=1;
    root->pred=nullptr;
    int next=1;
    root->order=next;
    stack<Node*> LIST;
    LIST.push(root);
    cout<<"Node: "<<root->label<<" order: "<<root->order<<endl;
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

void printTree(Node* root){
    for(Node* n : root->edges){
        if(n->pred==root){
            cout<<"Edge "<<root->label<<" - "<<n->label<<endl;
            printTree(n);
        }
    }
}

int main(int argc, char** argv) {
    bool treeMode = false;
    bool directed = true;
    if(argc != 3) {
        cerr << "Usage: " << argv[0] << " <integer>" << endl;
        return 1;
    }
    if(string(argv[1]) == "--tree") {
        cout << "Tree mode activated." << endl;
        treeMode = true;
    }
    if(string(argv[2]) == "--undirected") {
        cout << "Directed mode deactivated. Going off script." << endl;
        directed = false;
    }
    vector<pair<int, int>> edges;

    //case 1
    // edges.push_back({1, 2});
    // edges.push_back({1, 3});
    // edges.push_back({2, 4});
    // edges.push_back({2, 5});
    // edges.push_back({3, 6});
    // edges.push_back({3, 5});
    // edges.push_back({4, 5});
    // edges.push_back({5, 6});
    // Node* root=buildGraph(edges,directed);
    

    //case 2
    // edges.push_back({1, 2});
    // edges.push_back({1, 4});
    // edges.push_back({2, 3});
    // edges.push_back({2, 6});
    // edges.push_back({3, 4});
    // edges.push_back({6, 7});
    // edges.push_back({6, 5});
    // edges.push_back({4, 8});
    // edges.push_back({7, 3});
    // edges.push_back({5, 8});
    // edges.push_back({5, 1});
    // edges.push_back({8, 7});
    // Node* root=buildGraph(edges,directed);

    //case 3
    edges.push_back({1, 2});
    edges.push_back({1, 5});
    edges.push_back({1, 3});
    edges.push_back({2, 4});
    edges.push_back({2, 5});
    edges.push_back({3, 5});
    edges.push_back({3, 6});
    edges.push_back({5, 6});
    edges.push_back({4, 8});
    edges.push_back({4, 5});
    edges.push_back({6, 7});
    edges.push_back({6, 9});
    edges.push_back({7, 5});
    edges.push_back({7, 4});
    edges.push_back({7, 8});
    edges.push_back({9, 7});
    edges.push_back({9, 8});
    Node* root=buildGraph(edges,directed);

    //BFS(root);
    DFS(root);
    if(treeMode){
        printTree(root);
    }
    
    return 0;
}