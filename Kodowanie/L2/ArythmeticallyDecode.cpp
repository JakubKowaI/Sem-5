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
        input.get(c);
        frac=frac+bitset<8>(c).to_string();
        input.get(c);
        frac=frac+bitset<8>(c).to_string();
        P.push_back(pair<char,long double>(z,binaryFracToDecimal(frac)));
    }
    sort(P.begin(), P.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

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
    for(auto const& it:P){
        cout<<it.first<<" : "<<F[it.first].second<<" - "<<F[it.first].first<<endl;
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
        //cout<<bitset<8>(c)<<endl;
    }
    //long double z=binaryFracToDecimal(tempRead);
    //cout<<tempRead<<endl;
    //
    //cout<<binaryFracToDecimal("01110010")<<endl;
    input.close();
    
    pair<long double,long double> range(0.0L, 1.0L);
    long double z = 0.0L;
    int bit_index = 0;
    int underflow = 0;

    // zainicjalizuj z z pierwszych np. 64 bitów
    int initBits = min(112, (int)tempRead.size());
    for (int i = 0; i < initBits; ++i) {
        z = z * 2.0L + (tempRead[i] == '1' ? 1.0L : 0.0L);
    }
    if (initBits > 0) z /= powl(2.0L, initBits);
    bit_index = initBits;
    //cout<<"Z: "<<tempRead.substr(0,initBits)<<endl;
    cout<<"Z: "<<z<<endl;
    for (int i = 0; i < n; ++i) {
        long double d = range.second - range.first;

        for (auto& [c,p] : P) {
            long double lowBound  = range.first + F[c].second * d;
            long double highBound = range.first + F[c].first  * d;
            if(n<=150){
                cout<<"Trying: "<<c<<" |"<<lowBound<<" : "<<highBound<<"| "<<z<<endl;
            }
            
            if (z >= lowBound && z < highBound) {
                char decoded = c;
                output << decoded;
                range.first=lowBound;
                range.second=highBound;
                //cout<<"Hit"<<endl;
                //cout<<"New range: "<<range.first<<" : "<<range.second<<endl;
                
                //cout<<"Z bin: "<<tempRead.substr(bit_index-initBits,bit_index)<<endl;
                while (true) {
                    //cout<<"Z bin: "<<tempRead.substr(bit_index-initBits,bit_index)<<endl;
                    if (range.second < 0.5L) {
                        // E1 – cały przedział w dolnej połowie
                        range.first *= 2.0L;
                        range.second *= 2.0L;
                        z *= 2.0L;
                        cout<<"z: "<<z<<endl;
                        cout<<"E1 "<<bit_index<<endl;
                        if (bit_index < (int)tempRead.size()){
                            bit_index=bit_index+1;
                            z = 2.0L * z + (tempRead[bit_index] == '1' ? 1.0L : 0.0L);
                        }
                    }
                    else if (range.first >= 0.5L) {
                        // E2 – cały przedział w górnej połowie
                        range.first = 2.0L * range.first - 1.0L;
                        range.second = 2.0L * range.second - 1.0L;
                        z = 2.0L * z - 1.0L;
                        cout<<"z: "<<z<<endl;
                        cout<<"E2 "<<bit_index<<endl;
                        if (bit_index < (int)tempRead.size()){
                            bit_index=bit_index+1;
                            z = 2.0L * z + (tempRead[bit_index] == '1' ? 1.0L : 0.0L);
                        }
                    }
                    else if (range.first >= 0.25L && range.second < 0.75L&&range.first<0.5L&&range.second>0.5L) {
                        // E3 – underflow
                        underflow++;
                        range.first = 2.0L * range.first - 0.5L;
                        range.second = 2.0L * range.second - 0.5L;
                        z = 2.0L * z - 0.5L;
                        cout<<"z: "<<z<<endl;

                        cout<<"E3 "<<bit_index<<endl;
                        if (bit_index < (int)tempRead.size()){
                            bit_index=bit_index+1;
                            z = 2.0L * z + (tempRead[bit_index] == '1' ? 1.0L : 0.0L);
                        }
                            

                        //cout<<"PoBIT"<<bit_index<<endl;
                    }
                    else {
                        //cout<<"Z bin: "<<tempRead.substr(bit_index-initBits,bit_index)<<endl;
                        break;
                    }
                    //cout<<"Z bin: "<<tempRead.substr(bit_index-initBits,bit_index)<<endl;
                    // Bezpiecznik
                    if (z < 0.0L) z = 0.0L;
                    if (z >= 1.0L) z = nextafterl(1.0L, 0.0L);
                }
                //cout<<"Z bin: "<<tempRead.substr(bit_index-initBits,bit_index)<<endl;
                break;
        }
    }
    }
    output.close();
    
   
    


   
    
    
    for (auto &pr : codes) cout << pr.first << " -> " << pr.second << endl;
    

}