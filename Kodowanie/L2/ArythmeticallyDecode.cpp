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

bool comp(pair<char,long double> a,pair<char,long double> b){
    return (int)get<0>(a) < (int)get<0>(b);
}

long double binaryFracToDecimal(const string& bits) {
    long double result = 0.0L;
    long double factor = 0.5L;  // pierwszy bit po przecinku = 2^-1

    for (char bit : bits) {
        if (bit == '1')
            result += factor;
        factor /= 2.0L;
    }

    return result;
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
    char c;
    int count = 0;
    
    

    string temp="";
    for(int i=0;i<4;i++){
        input.get(c);
        temp=temp+bitset<8>(c).to_string();
    }
    u_int Pn =(u_int)bitset<32>(temp).to_ulong();
    cout<<"Pn: "<<Pn<<endl;
    vector<pair<char,long double>> P;
    for(int i=0;i<Pn;i++){
        input.get(c);
        char z=c;
        string frac;
        input.get(c);
        frac=frac+bitset<8>(c).to_string();
        input.get(c);
        frac=frac+bitset<8>(c).to_string();
        P.push_back(pair<char,long double>(z,binaryFracToDecimal(frac)));
    }
    // sort(P.begin(),P.end(),comp);
    for(auto it:P){
        cout<<it.first<<" : "<<it.second<<endl;
    }

    unordered_map<char,pair<long double,long double>> F;
    F[P.begin()->first]=pair<long double,long double>(P.begin()->second,0.0);
    long double upToThis=P.begin()->second;
    for(auto it:P){
        //cout<<"F dla "<<it.first<<endl;
        if(it==*P.begin())continue;
        long double Fminus = upToThis;
        upToThis+=it.second;
        F[it.first]=pair<long double,long double>(upToThis,Fminus);
        //cout<<"F(i+1): "<<upToThis<<" F(i): "<<Fminus<<endl;
    }

    temp="";
    for(int i=0;i<4;i++){
        input.get(c);
        temp=temp+bitset<8>(c).to_string();
    }
    u_int n =(u_int)bitset<32>(temp).to_ulong();
    // cout<<temp<<endl;
    cout<<"N: "<<n<<endl;
    ofstream output("ArythmicallyDecompressed.txt");

    string tempRead;
    while(input.get(c)){
        tempRead=tempRead+bitset<8>(c).to_string();
    }
    long double z=binaryFracToDecimal(tempRead);
    cout<<tempRead<<endl;
    cout<<"Z: "<<z<<endl;
    //cout<<binaryFracToDecimal("01110010")<<endl;
    input.close();
    
    pair<long double,long double> range(0.0,1.0);
    for(int i=0;i<n;i++){
        for(auto it:F){
            if ((range.first + it.second.second * (range.second - range.first) <= z)
                && (z < range.first + it.second.first * (range.second - range.first))) {
                    cout<<range.first + it.second.second * (range.second - range.first)<<" : "<<range.first + it.second.first * (range.second - range.first)<<" - "<<z<<endl;
                output<<it.first;
                long double d=range.second-range.first;
                range.second=range.first+it.second.first*d;
                range.first=range.first+it.second.second*d;
                break;
            }
        }
    }
    
    
    output.close();
    
   
    


   
    
    
    for (auto &pr : codes) cout << pr.first << " -> " << pr.second << endl;
    

}