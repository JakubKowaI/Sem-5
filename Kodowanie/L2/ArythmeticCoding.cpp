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
    return get<1>(a) > get<1>(b);
}

string fracToBinary(long double n, int m) {
    if (n >= 1.0L) n -= static_cast<unsigned long long>(n);
    if (n < 0.0L) n = -n;

    string bits;
    for (int i = 0; i < m; ++i) {
        n *= 2.0L;
        if (n >= 1.0L) {
            bits += '1';
            n -= 1.0L;
        } else {
            bits += '0';
        }
    }
    return bits;
}

string bitset32ToSave(bitset<32> x){
    string t;
    // cout<<"Start "<<x<<endl;
    for(int i=3;i>=0;i--){
        bitset<8> temp;
        for(int j=0;j<8;j++){
            temp[j]=x[8*i+j];
        }
        // cout<<temp<<endl;
        char z=(char)temp.to_ulong();
        t=t+z;
    }
    return t;
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

string bitset16ToSave(bitset<16> x){
    string t;
    for(int i=1;i>=0;i--){
        bitset<8> temp;
        for(int j=0;j<8;j++){
            temp[j]=x[8*i+j];
        }
        char z=(char)temp.to_ulong();
        t=t+z;
    }
    return t;
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
    u_int count = 0;
    unordered_map<char, int> freq;

    while (input.get(c)) {
        freq[c]++;
        count++;
    }
    input.close();
    
    
    vector<pair<char,long double>> P;
    for(auto [key, value]:freq){
        P.push_back(pair<char,double>(key,(double)value/count));
    }
    // P.push_back(pair<char,long double>('A',0.7));
    // P.push_back(pair<char,long double>('B',0.1));
    // P.push_back(pair<char,long double>('C',0.2));

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
    for(auto it:F){
        cout<<it.first<<" . "<<it.second.first<<" . "<<it.second.second<<endl;
    }
    long double entropy1 = 0.0;

    for (auto& it : freq) {
        long double p = (long double)it.second / count;
        long double info = -log2(p);
        entropy1 += p * info;
    }
    
    ofstream output("ArythmeticallyCompressed.bin");

    ifstream input2(filename, ios::binary);
    if (!input2.is_open()) {
        cerr << "File not found: " << endl;
        return -1;
    }

    pair<long double,long double> range(0.0,1.0);
    long double Px=0.0;
    int underflow = 0;
    string op;
    int itt=0;
    while (input2.get(c)) {
        if(itt<100)
            std::cout<<"Przed: "<<range.first<<" , "<<range.second<<" Char: "<<c<<endl;
        Px+=F[c].first-F[c].second;
        long double d=range.second-range.first;
        // cout<<"D: "<<d<<endl;
        range.second=range.first+F[c].first*d;
        // cout<<"Right: "<<range.second<<endl;
        range.first=range.first+F[c].second*d;
        // cout<<"Left: "<<range.first<<" F(i) "<<F[c].second<<endl;
        // cout<<"Char: "<<c<<" - "<<F[c].first<<" - "<<F[c].second<<endl;
        if(itt<100)
            std::cout<<"Po: "<<range.first<<" , "<<range.second<<endl;
        itt++;
        
        while(true){
            if(range.first<0.5&&range.second<0.5){
                
                op.push_back('0');
                for (; underflow > 0; underflow--) op.push_back('1'); // "odwrócone" bity z underflow
                
                
                range.first *= 2;
                range.second *= 2;
            }else if(range.first>=0.5&&range.second<1){
                op.push_back('1');
                for (; underflow > 0; underflow--) op.push_back('0');
                
                
                range.first = 2 * range.first - 1;
                range.second = 2 * range.second - 1;
            }else if((range.first<0.5&&range.second>0.5)&&(range.first>0.25&&range.second<0.75)){
                underflow++;
                range.first = 2 * range.first - 0.5;
                range.second = 2 * range.second - 0.5;
            }else{
                break;
            }
            if(itt<100)
        std::cout<<"PoPo: "<<range.first<<" , "<<range.second<<endl;

        }

        
    }
    cout<<range.first<<" : "<<range.second<<endl;

    long double z=(range.first+range.second)/2;
    cout<<"Z: "<<z<<endl;
    int mx=ceil(log(1/Px))+1;
    if(mx%8!=0){
        mx+=8-mx%8;
    }
    string zapis=bitset32ToSave(bitset<32>(P.size()));
    //cout<<"Zapis: "<<zapis<<endl;
    output<<zapis;
    for(auto it:P){
        output<<it.first;
        string p =fracToBinary(it.second,16);
        output<<bitset16ToSave(bitset<16>(p));
    }
    string out=fracToBinary(z,mx);
    output<<bitset32ToSave(bitset<32>(count));
    //cout<<"Z: "<<binaryFracToDecimal(out)<<" : "<<z<<endl;
    string realOut;
    //cout<<"Out: "<<out<<endl;
    for(int i=0;i<out.size()/8;i++){
        bitset<8>temp;
        for(int j=0;j<8;j++){
            //cout<<out[8*i+j]<<endl;
            temp[7-j] = (out[8*i+j] == '1');
        }
        //cout<<temp<<endl;
        realOut=realOut+(char)bitset<8>(temp.to_string()).to_ulong();
    }
    //cout<<"RealOut: "<<realOut<<endl;
    output<<realOut;
    input2.close();
    output.close();
    cout<<z<<endl;
    cout<<"Entropia: "<<entropy1<<endl;
}