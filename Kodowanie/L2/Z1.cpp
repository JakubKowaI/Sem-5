#include <iostream>
#include <bitset>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
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
int hmax;
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
        if(prefix.empty()) cout<<node->key<<" : "<<"0"<<endl;
        else cout<<node->key<<" : "<<prefix<<endl;
    }
}

Node* huffman(vector<pair<char,double>> P){
    struct Cmp {
        bool operator()(const Node* a, const Node* b) const {
            if (a->p != b->p) return a->p < b->p; 
            return a < b; 
        }
    };

    multiset<Node*, Cmp> q;
    multiset<Node*, Cmp> after; 
    for (auto &pr : P) {
        q.insert(new Node(pr.first, pr.second));
    }
    // for(auto it:q){
    //     cout<<it->p<<it->key<<endl;
    // }
    
        while (q.size() > 1) {
            
            after.clear();
            auto it1 = q.begin();
            Node* n1 = *it1;
            q.erase(it1);

            auto it2 = q.begin();
            Node* n2 = *it2;
            q.erase(it2);
            cout<<"lacze: "<<n1->key<<" ^ "<<n2->key<<endl;
            Node* parent;
            if(n1->p<n2->p){
                parent = new Node(n1->p + n2->p, n1, n2);
            }else{
                parent = new Node(n1->p + n2->p, n2, n1);
            }
            q.insert(parent);
        }

    Node* root = q.empty() ? nullptr : *q.begin();
    //if (root) printTree(root, "");
    return root;
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
    unordered_map<char,string> codes;
    if (kod!=nullptr) {
        vector<pair<Node*, string>> st;
        st.emplace_back(kod, "");
        while (!st.empty()) {
            auto [node, pref] = st.back();
            st.pop_back();
            if (!node) continue;
            if (!node->branch) {
                codes[node->key] = pref.empty() ? string("0") : pref;
            } else {
                if (node->right) st.emplace_back(node->right, pref + "1");
                if (node->left)  st.emplace_back(node->left,  pref + "0");
            }
        }
    }
    for (auto &pr : codes) cout << pr.first << " -> " << pr.second << endl;
    ofstream output("compressed.txt");
    output<<codes.size()<<endl;
    for(auto el: codes){
        output<<el.first<<" "<<el.second<<endl;
    }
    ifstream input2(filename, ios::binary);
    if (!input2.is_open()) {
        cerr << "File not found: " << endl;
        return -1;
    }
    while (input2.get(c)) {
        output<<codes[c];
    }
    input2.close();

    cout<<"Entropia: "<<entropy1<<endl;
}