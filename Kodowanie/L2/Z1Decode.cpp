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
    string s;
    getline(input,s);
    n=stoi(s);
    for(int i=0;i<n;i++){
        input.get(c);
        getline(input,s);
        codes[c]=s.substr(1);
    }
    for(auto it:codes){
        cout<<"Key: "<<it.first<<" value: "<<it.second<<endl;
    }
    string temp="";
    string lookup="";
    ofstream output("decompressed.txt");
    while (input.get(c)) {
        
        temp=temp+c;
        if(temp[0]=='\n')temp= temp.substr(1);
        //cout<<"Char: "<<c<<" Temp: "<<temp<<endl;
        // auto it = find_if(codes.begin(), codes.end(),
        //                 [&temp](auto&& p) { return p.second == temp; });
        for(auto it:codes){
            //cout<<"code: "<<it.second<<" Temp: "<<temp<<endl;
            if(!it.second.compare(temp)){
                output<<it.first;
                temp="";
            }
        }
        // if(it!=codes.end()){
        //     output<<it->first;
        //     cout<<"Bingo"<<endl;
        //     //temp=temp.substr(temp.size());
        //     temp="";
        // }
    
    }
    input.close();
    output.close();
    
   
    


   
    
    
    for (auto &pr : codes) cout << pr.first << " -> " << pr.second << endl;
    

}