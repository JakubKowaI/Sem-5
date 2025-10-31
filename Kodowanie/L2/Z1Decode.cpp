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

struct DecodeNode {
    char symbol{};
    DecodeNode* left{nullptr};
    DecodeNode* right{nullptr};
};

DecodeNode* buildHuffmanTree(const unordered_map<char, string>& codes) {
    auto root = new DecodeNode();
    for (const auto& [symbol, code] : codes) {
        DecodeNode* cur = root;
        for (char bit : code) {
            DecodeNode*& next = (bit == '0') ? cur->left : cur->right;
            if (!next) next = new DecodeNode();
            cur = next;
        }
        cur->symbol = symbol;
    }
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
    unordered_map<char,string> codes;
    int n;
    char c;
    int count = 0;
    
    input.get(c);
    string firstSize=bitset<8>(c).to_string();
    input.get(c);
    firstSize=firstSize+bitset<8>(c).to_string();
    n=bitset<16>(firstSize).to_ulong();
    cout<<"n - "<<bitset<16>(n)<<endl;
    for(int i=0;i<n;i++){
        input.get(c);
        char len;
        input.get(len);
        int lenght = (u_int8_t)len;
        char binTemp;
        string rep="";
        for(int k=0;k<(lenght/8)+1;k++){
            input.get(binTemp);
            bitset<8> binRep(binTemp);
            if((k*8)+8>lenght){
                rep=rep+binRep.to_string().substr(0,lenght-k*8);
            }else{
                rep=rep+binRep.to_string();
            }
        }
        codes[c]=rep;
    }
    for(auto it:codes){
        cout<<"Key: "<<it.first<<" value: "<<it.second<<endl;
    }
    string temp="";
    string lookup="";
    ofstream output("decompressed.txt");
    // while (input.get(c)) {
    //     bitset<8> binTemp(c);
    //     //string stemp=c.to_string();
    //     temp=temp+binTemp.to_string();
    //     size_t i = 0;
    //     bool matched = false;
    //     do{
    //         matched=false;
    //         while (i < temp.size()) {

            
                
    //             for (auto &it : codes) {
    //                 if (i + it.second.size() <= temp.size()) {
    //                     if (!it.second.compare(temp.substr(i, it.second.size()))) {
    //                         output << it.first;
    //                         temp = temp.substr(i + it.second.size());
    //                         i = 0; // restart od nowego początku
    //                         matched = true;
    //                         break;
    //                     }
    //                 }
    //             }
    //             if(matched){
    //                 break;
    //             }else{
    //                 i++;
    //             }
            

            
    //         }
    //     }while(!matched);
         
    // }

    DecodeNode* root = buildHuffmanTree(codes);
    DecodeNode* node = root;

    unsigned char byte;
    while (input.read(reinterpret_cast<char*>(&byte), 1)) {
        for (int mask = 0x80; mask; mask >>= 1) {
            node = (byte & mask) ? node->right : node->left;
            if (!node) throw runtime_error("invalid bit pattern");

            if (!node->left && !node->right) {
                output.put(node->symbol);
                node = root;
            }
        }
    }

    string tempRead;
    while(input.get(c)){
        tempRead=tempRead+bitset<8>(c).to_string();
        for(int i=0;i<tempRead.size();i++){
            if(tempRead[i]=='1'&&node->right){
                node= node->right;
            }else{
                node= node->left;
            }
            if(!node->left&&!node->right){
                output<<node->symbol;
                node=root;
                tempRead=tempRead.substr(i);
                i=-1;
            }
        }
    }


    input.close();
    output.close();
    
   
    


   
    
    
    for (auto &pr : codes) cout << pr.first << " -> " << pr.second << endl;
    

}