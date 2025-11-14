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

// long double F(int n,const vector<pair<char,long double>> P){
//     long double sum=0.0L;
//     for(int i=0;i<n;i++){
//         sum+=P[i].second;
//     }
//     return sum;
// }

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
    sort(P.begin(), P.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    unordered_map<char,pair<long double,long double>> F;
    long double upToThis=0.0;
    for(auto const& it:P){
        long double Fminus = upToThis;
        upToThis+=it.second;
        F[it.first]=pair<long double,long double>(upToThis,Fminus);
    }
    for(auto const& it:P){
        cout<<it.first<<" . "<<F[it.first].first<<" . "<<F[it.first].second<<endl;
    }
    long double entropy1 = 0.0;

    for (auto& it : freq) {
        long double p = (long double)it.second / count;
        long double info = -log2(p);
        entropy1 += p * info;
    }
    
    ofstream output("ArythmeticallyCompressed.bin", ios::binary);

    ifstream input2(filename, ios::binary);
    if (!input2.is_open()) {
        cerr << "File not found: " << endl;
        return -1;
    }

    pair<long double,long double> range(0.0,1.0); //Kodowanie
    int underflow = 0;
    string op;
    
    while (input2.get(c)) {
        long double d = range.second - range.first;
        range.second = range.first + d * F[c].first;
        range.first = range.first + d * F[c].second;
        
        while (true) {
            if (range.second < 0.5) {
                // E1
                op += '0';
                for (int i = 0; i < underflow; ++i) op += '1';
                underflow = 0;
                range.first *= 2.0;
                range.second *= 2.0;
            } else if (range.first >= 0.5) {
                // E2
                op += '1';
                for (int i = 0; i < underflow; ++i) op += '0';
                underflow = 0;
                range.first = 2.0 * range.first - 1.0;
                range.second = 2.0 * range.second - 1.0;
            } else if (range.first >= 0.25 && range.second < 0.75) {
                // E3
                underflow++;
                range.first = 2.0 * range.first - 0.5;
                range.second = 2.0 * range.second - 0.5;
            } else {
                break;
            }
        }
    }
    input2.close();
    
    underflow++;
    if (range.first < 0.25) {
        op += '0';
        for (int i = 0; i < underflow; ++i) op += '1';
    } else {
        op += '1';
        for (int i = 0; i < underflow; ++i) op += '0';
    }
    
    cout<<"Dlugosc: "<<op.length()<<endl;
    cout<<"H: "<<entropy1<<endl;
    cout<<"Srednia dlugosc: "<<(double)op.length()/count<<endl;

    while(op.length()%8!=0) op=op+'0';

    //Zapis

    string zapis=bitset32ToSave(bitset<32>(P.size()));
    output<<zapis;
    for(auto it:P){
        output<<it.first;
        string p =fracToBinary(it.second,32);
        output<<bitset32ToSave(bitset<32>(p));
    }

    output<<bitset32ToSave(bitset<32>(count));
    cout<<"Pn: "<<P.size()<<endl;
    //long mx=ceil(log(1/Px))+1;
    
    string realOut;
    //op+=fracToBinary(z,128);
    //if(mx>0)
    //    op=op.substr(0,mx);

    for(int i=0;i<op.size()/8;i++){
        bitset<8>temp;
        for(int j=0;j<8;j++){
            temp[7-j] = (op[8*i+j] == '1');
        }
        realOut=realOut+(char)bitset<8>(temp.to_string()).to_ulong();
    }
    if (op.size() % 8 != 0) {
        bitset<8> temp;
        for (int j = 0; j < op.size() % 8; ++j) {
            temp[7 - j] = (op[op.size() / 8 * 8 + j] == '1');
        }
        realOut += (char)temp.to_ulong();
    }
    
    cout<<"Z: "<<binaryFracToDecimal(op)<<endl;
    //cout<<"N: "<<n<<endl;
    //cout<<"Op: "<<op<<endl;
    //cout<<"mx: "<<mx<<endl;
    output<<realOut;
    input2.close();
    output.close();
    cout<<"Entropia: "<<entropy1<<endl;
}