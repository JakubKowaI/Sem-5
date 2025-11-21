#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <bitset>

using namespace std;

string eliasY(u_int64_t x) {
    if (x == 0) throw runtime_error("Gamma undefined for 0");
    string bin = bitset<64>(x).to_string();
    bin.erase(0, bin.find('1'));

    string prefix(bin.size() - 1, '0');
    return prefix + bin;
}

u_int64_t eliasYDecode(const string &s) {
    int zeros = 0;
    for(int i=0;i<s.size()&&s[i]=='0';i++){
        zeros++;
    }

    string bin = s.substr(zeros, zeros + 1);

    return stoull(bin, nullptr, 2);
}

string eliasO(u_int64_t x) {
    if (x == 0) throw runtime_error("Delta undefined for 0");
    string bin = bitset<64>(x).to_string();
    bin.erase(0, bin.find('1'));

    u_int64_t len = bin.size();
    string gammaLen = eliasY(len);

    return gammaLen + bin.substr(1);
}

u_int64_t eliasODecode(const string &s) {
    u_int64_t n = eliasYDecode(s);
    //k-1,n,x

    string rest(s.rbegin(),s.rbegin()+n-1);
    reverse(rest.begin(),rest.end());
    string bin = "1" + rest;
    return stoull(bin, nullptr, 2);
}

string eliasW(u_int64_t x){
    string w="0";
    u_int64_t k=x;
    while(k>1){
        string bin = bitset<64>(k).to_string();
        bin.erase(0, bin.find('1'));
        k=bin.length()-1;
        w=bin+w;
    }
    return w;
}

u_int64_t eliasWDecode(const string &s) {
    u_int64_t n = 1;
    size_t pos = 0;

    while (pos < s.size()) {
        if (s[pos] == '0') return n;

        u_int64_t chunkLen = n + 1;
        if (pos + chunkLen > s.size()) {
            throw runtime_error("Niepełny kod Eliasza-omega");
        }

        string bin = s.substr(pos, chunkLen);
        n = stoull(bin, nullptr, 2);
        pos += chunkLen;
    }
}

string fibonacci(u_int64_t x) {
    if (x == 0) throw runtime_error("Nie ma fib dla 0");

    vector<u_int64_t> F = {1, 2};
    while (F.back() <= x)
        F.push_back(F[F.size() - 1] + F[F.size() - 2]);

    F.pop_back();

    string code(F.size(), '0');

    for (int i = (int)F.size() - 1; i >= 0; i--) {
        if (F[i] <= x) {
            x -= F[i];
            code[i] = '1';
        }
    }

    code.push_back('1');

    return code;
}

u_int64_t fibonacciDecode(const string &s) {
    vector<u_int64_t> F = {1, 2};
    while (F.size() < s.size())
        F.push_back(F[F.size() - 1] + F[F.size() - 2]);

    u_int64_t value = 0;
    int idx = 0;

    for(int i=0;i<s.size();i++){
        if(s[i]=='1') value+=F[i];
    }
    return value-=F.back();
}

auto pickCode(char mode){
    switch (mode)
    {
    case 'w':
        return eliasW;
        break;
    case 'y':
        return eliasY;
        break;
    case 'o':
        return eliasO;
        break;
    case 'f':
        return fibonacci;
        break;
    default:
        cout<<"Zla metoda kodowania uzywam eliasw"<<endl;
        break;
    }
}

void wypisz(string &buffer,ofstream &output){
    while(buffer.length()>=8){
        string temp=buffer.substr(0,8);
        buffer.erase(0,8);
        bitset<8> out(temp);
        output<<(char)out.to_ulong();
    }
}

int main(int argc,char** argv){
    if (argc < 2) {
        cerr << "Using: " << argv[0] << endl;
        return -1;
    }

    char mode='w';
    if(argc==3){
        mode=argv[2][0];
    }

    string (*Method)(u_int64_t x);
    Method=pickCode(mode);

    string filename = argv[1];
    ifstream input(filename, ios::binary);
    ofstream output("LZWCoded.bin");
    cout<<mode<<endl;
    output<<mode;
    vector<string> slownik;

    for (int i = 0; i < 256; ++i) slownik.push_back(string(1, static_cast<char>(i)));
    
    char c=input.get();
    string buf=""+c;
    string outBuf="";
    while(input.get(c)){
        buf=buf+c;
        auto f = find(slownik.begin(),slownik.end(),buf);
        if(f!=slownik.end()){
            continue;
        }else{
            u_int16_t index=distance(slownik.begin(),f);
            //output<<bitset<16>(index).to_string();
            outBuf=outBuf+Method(index);
            wypisz(outBuf,output);
            slownik.push_back(buf);
            buf=""+c;
        }
    }
    auto f = find(slownik.begin(),slownik.end(),buf);
        
    u_int16_t index=distance(slownik.begin(),f);
    //output<<bitset<16>(index).to_string();
    outBuf=outBuf+Method(index);
    wypisz(outBuf,output);
            
    output.close();
    input.close();
    return 0;
}