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
    }
    input.close();
    
    pair<long double,long double> range(0.0L, 1.0L);
    
    const int PRECISION = 60;
    unsigned long long z_val = 0;
    if (tempRead.length() > 0) {
        z_val = stoull(tempRead.substr(0, min((size_t)PRECISION, tempRead.length())), nullptr, 2);
    }
    int bit_index = min((size_t)PRECISION, tempRead.length());

    for (int i = 0; i < n; ++i) {
        long double d = range.second - range.first;
        unsigned long long z_comp = (unsigned long long)((z_val - range.first * powl(2.0L, PRECISION)) / d);

        char decoded_char = ' ';
        for (auto& [c,p] : P) {
            long double low_prob_bound = F[c].second;
            long double high_prob_bound = F[c].first;
            
            if (z_comp >= (unsigned long long)(low_prob_bound * powl(2.0L, PRECISION)) && z_comp < (unsigned long long)(high_prob_bound * powl(2.0L, PRECISION))) {
                decoded_char = c;
                output << decoded_char;
                range.first = range.first + d * low_prob_bound;
                range.second = range.first + d * high_prob_bound;
                break;
            }
        }

        while (true) {
            if (range.second < 0.5) {
                range.first *= 2.0;
                range.second *= 2.0;
                z_val = (z_val << 1);
                if (bit_index < tempRead.length()) {
                    z_val |= (tempRead[bit_index] - '0');
                    bit_index++;
                }
            } else if (range.first >= 0.5) {
                range.first = 2.0 * range.first - 1.0;
                range.second = 2.0 * range.second - 1.0;
                z_val = (z_val - (1ULL << (PRECISION -1))) << 1;
                 if (bit_index < tempRead.length()) {
                    z_val |= (tempRead[bit_index] - '0');
                    bit_index++;
                }
            } else if (range.first >= 0.25 && range.second < 0.75) {
                range.first = 2.0 * range.first - 0.5;
                range.second = 2.0 * range.second - 0.5;
                z_val = (z_val - (1ULL << (PRECISION -2))) << 1;
                 if (bit_index < tempRead.length()) {
                    z_val |= (tempRead[bit_index] - '0');
                    bit_index++;
                }
            } else {
                break;
            }
        }
    }
    output.close();
    
   
    


   
    
    
    for (auto &pr : codes) cout << pr.first << " -> " << pr.second << endl;
    

}