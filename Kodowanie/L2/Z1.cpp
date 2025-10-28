#include <iostream>
#include <bitset>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <utility>
#include <string>

using namespace std;

bool comp(pair<char,double> a,pair<char,double> b){
    return get<1>(a) > get<1>(b);
}

struct Node{
char key;
double p;
bool branch=0;
Node* right=nullptr;
Node* left=nullptr;
Node(char k, double p){
    this->key=k;
    this->p=p;
}
Node(double p,Node* a, Node* b){
    this->p=p;
    this->branch=1;
    this->left=a;
    this->right=b;
}
};

Node* joinTrees(Node *a, Node *b){
    if (a->right != nullptr && b->p < a->right->p) {
        a->right = new Node(a->right->p + b->p, a->right, b);
        return a;
    }
    return new Node(a->p + b->p, a, b);
}

void printTree(Node* node,string prefix){
    if(node==nullptr)return;
    if(node->branch){
        printTree(node->right,prefix+"1");
        printTree(node->left,prefix+"0");
    }else{
        cout<<node->key<<" : "<<prefix<<endl;
    }
}

Node* huffman(vector<pair<char,double>> P){
    vector<Node*> treePointers;
    treePointers.reserve(P.size());
    for(auto [key,value]:P){
        Node* leaf = new Node(key, value);
        treePointers.push_back(leaf);
    }
    while (treePointers.size()>1)
    {
        Node* a=treePointers.back();
        treePointers.pop_back();
        Node* b=treePointers.back();
        treePointers.pop_back();
        treePointers.push_back(joinTrees(b,a));
    }
    if(!treePointers.empty()){
        printTree(treePointers[0],"");
    }
    return treePointers[0];
}

int main(int argc,char** argv){
    if (argc < 2) {
        cerr << "Using: " << argv[0] << endl;
        return -1;
    }

    string filename = argv[1];
    ifstream input(filename, ios::binary);
    if (!input.is_open()) {
        cerr << "File not found: " << endl;
        return -1;
    }

    char c;
    int count = 0;
    unordered_map<char, int> freq;

    while (input.get(c)) {
        freq[c]++;
        count++;
    }
    input.close();
    
    vector<pair<char,double>> P;
    for(auto [key, value]:freq){
        P.push_back(pair<char,double>(key,(double)value/count));
    }
    
    sort(P.begin(),P.end(),comp);

    long double entropy1 = 0.0;

    for (auto& it : freq) {
        long double p = (long double)it.second / count;
        long double info = -log2(p);
        entropy1 += p * info;
    }
    Node* kod = huffman(P);
    cout<<"Entropia: "<<entropy1<<endl;
}