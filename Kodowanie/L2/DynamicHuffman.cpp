#include <iostream>
#include <bitset>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <queue>
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
int w;
bool branch=0;
bool isNyt=false;
Node* right=nullptr;
Node* left=nullptr;
Node* parent=nullptr;
int h;
int x;
int hmax;
Node(char k, int w,Node* parent){
    this->key=k;
    this->w=w;
    this->parent=parent;
}
Node(int w,Node* a, Node* b,Node* p){
    this->w=w;
    this->branch=1;
    this->left=a;
    this->right=b;
    this->parent=p;
}
};
Node* getRoot(Node* node){
    while(node && node->parent){
        node=node->parent;
    }
    return node;
}

bool isAncestor(Node* ancestor,Node* node){
    while(node){
        if(node==ancestor) return true;
        node=node->parent;
    }
    return false;
}

void replaceChild(Node* parent,Node* oldChild,Node* newChild){
    if(!parent) return;
    if(parent->left==oldChild){
        parent->left=newChild;
    }else if(parent->right==oldChild){
        parent->right=newChild;
    }
}

Node* findHighestInBlock(Node* root,int weight,Node* current){
    if(!root) return nullptr;
    queue<Node*> q;
    q.push(root);
    Node* candidate=nullptr;
    while(!q.empty()){
        Node* node=q.front();
        q.pop();
        if(node->w==weight && node!=current && node->x>current->x && !isAncestor(node,current) && !isAncestor(current,node)){
            if(!candidate || node->x>candidate->x){
                candidate=node;
            }
        }
        if(node->left) q.push(node->left);
        if(node->right) q.push(node->right);
    }
    return candidate;
}

void swapNodes(Node* a,Node* b){
    if(!a || !b || a==b) return;
    Node* parentA=a->parent;
    Node* parentB=b->parent;
    replaceChild(parentA,a,b);
    replaceChild(parentB,b,a);
    a->parent=parentB;
    b->parent=parentA;
    std::swap(a->x,b->x);
}

Node* createNYT(){
    Node* node = new Node('\0',0,nullptr);
    node->isNyt = true;
    node->branch = false;
    return node;
}

Node* NYT=createNYT();

Node* expandNYT(char symbol){
    Node* oldNYT = NYT;
    oldNYT->branch = true;
    oldNYT->isNyt = false;
    oldNYT->key = '\0';

    Node* newNYT = new Node('\0',0,oldNYT);
    newNYT->isNyt = true;
    Node* newLeaf = new Node(symbol,0,oldNYT);
    newLeaf->isNyt = false;

    oldNYT->left = newNYT;
    oldNYT->right = newLeaf;

    newLeaf->x = oldNYT->x - 1;
    newNYT->x = oldNYT->x - 2;

    NYT = newNYT;
    return newLeaf;
}

Node* search(Node* node,char k){
    if(!node) return nullptr;
    if(!node->branch && !node->isNyt && node->key==k) return node;
    Node* left=nullptr;
    Node* right=nullptr;
    if(node->left){
        left=search(node->left,k);
        if(left) return left;
    }
    if(node->right){
        right=search(node->right,k);
        if(right) return right;
    }
    return nullptr;
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

// Node* huffman(vector<pair<char,double>> P){
//     struct Cmp {
//         bool operator()(const Node* a, const Node* b) const {
//             if (a->p != b->p) return a->p < b->p; 
//             return a < b; 
//         }
//     };

//     multiset<Node*, Cmp> q;
//     multiset<Node*, Cmp> after; 
//     for (auto &pr : P) {
//         q.insert(new Node(pr.first, pr.second));
//     }
//     // for(auto it:q){
//     //     cout<<it->p<<it->key<<endl;
//     // }
    
//         while (q.size() > 1) {
            
//             after.clear();
//             auto it1 = q.begin();
//             Node* n1 = *it1;
//             q.erase(it1);

//             auto it2 = q.begin();
//             Node* n2 = *it2;
//             q.erase(it2);
//             //cout<<"lacze: "<<n1->key<<" ^ "<<n2->key<<endl;
//             Node* parent;
//             if(n1->p<n2->p){
//                 parent = new Node(n1->p + n2->p, n1, n2);
//             }else{
//                 parent = new Node(n1->p + n2->p, n2, n1);
//             }
//             q.insert(parent);
//         }

//     Node* root = q.empty() ? nullptr : *q.begin();
//     //if (root) printTree(root, "");
//     return root;
// }

void fixUp(Node* start){
    Node* node=start;
    while(node){
        Node* root=getRoot(node);
        Node* candidate=findHighestInBlock(root,node->w,node);
        if(candidate){
            swapNodes(node,candidate);
        }
        node->w++;
        node=node->parent;
    }
}

void addToCode(Node*& root,char k){
    Node* node=search(root,k);
    if(node){
        fixUp(node);
    }else{
        Node* newLeaf = expandNYT(k);
        fixUp(newLeaf);
    }
    root = getRoot(root);
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
    Node* root=NYT;
    root->x=255;
    while (input.get(c)) {
        freq[c]++;
        count++;
        addToCode(root,c);
    }
    input.close();

    long double entropy1 = 0.0;

    for (auto& it : freq) {
        long double p = (long double)it.second / count;
        long double info = -log2(p);
        entropy1 += p * info;
    }
    //Node* kod = huffman(P);
    unordered_map<char,string> codes;
    cout<<"test"<<endl;
    if (root!=nullptr) {
        vector<pair<Node*, string>> st;
        st.emplace_back(root, "");
        while (!st.empty()) {
            auto [node, pref] = st.back();
            st.pop_back();
            if (!node) continue;
            if (!node->branch && !node->isNyt) {
                codes[node->key] = pref.empty() ? string("0") : pref;
            } else {
                if (node->right) st.emplace_back(node->right, pref + "1");
                if (node->left)  st.emplace_back(node->left,  pref + "0");
            }
        }
    }
    for (auto &pr : codes) cout << pr.first << " -> " << pr.second << endl;
    ofstream output("DynamicallyCompressed.bin");
    string size=bitset<16>(codes.size()).to_string();
    cout<<codes.size()<<" bin: "<<size<<endl;
    //bitset<8> k(size.substr(0,8));
    output<<(char)bitset<8>(size.substr(0,8)).to_ulong()<<(char)bitset<8>(size.substr(8,16)).to_ulong();
    for(auto el: codes){
        output<<el.first<<(char)el.second.size();
        //cout<<"Koduje "<<el.first<<" jako: "<<el.second<<" "<<el.second.size()<<endl;
        for(int i=0;i<(el.second.size()/8)+1;i++){
            if((8*i)+8<=el.second.size()){
                output<<(char)bitset<8>(el.second.substr(8*i,(8*i)+8)).to_ulong();
                //cout<<bitset<8>(el.second.substr(8*i,(8*i)+8));
            }else{
                string tt=el.second.substr(8*i);
                //cout<<tt<<endl;
                while(tt.size()<8){
                    tt=tt+"0";
                }
                output<<(char)bitset<8>(tt).to_ulong();
                //cout<<bitset<8>(tt);
            }
        }
        //cout<<endl;
    }
    ifstream input2(filename, ios::binary);
    if (!input2.is_open()) {
        cerr << "File not found: " << endl;
        return -1;
    }
    string przerob="";
    while (input2.get(c)) {
        przerob=przerob+ codes[c];
        //cout<<"Coding: "<<c<<" to temp: "<<przerob<<endl;
        if(przerob.size()>=8){
            char zapis=(char)bitset<8>(przerob.substr(0,8)).to_ulong();
            output<<zapis;
            //cout<<"Zapis: "<<bitset<8>(zapis)<<endl;
            if(przerob.size()==8){
                przerob="";
            }else{
                przerob=przerob.substr(8);
            }
        }
        //char temp =(char)bitset<8>(codes[c]).to_ulong();
        //cout<<bitset<8>(temp)<<endl;
    }
    while(przerob.size()>8){
        output<<(char)bitset<8>(przerob.substr(0,8)).to_ulong();
            if(przerob.size()==8){
                przerob="";
            }else{
                przerob=przerob.substr(8);
            }
    }
    if(przerob.size()>0){
        while(przerob.size()<8){
            przerob=przerob+"0";
        }
    }
    output<<(char)bitset<8>(przerob.substr(0,8)).to_ulong();
    input2.close();

    cout<<"Entropia: "<<entropy1<<endl;
}