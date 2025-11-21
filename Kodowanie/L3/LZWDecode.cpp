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

u_int64_t eliasYDecode(string &s) {
    int zeros = 0;
    for(int i=0;i<s.size()&&s[i]=='0';i++){
        zeros++;
    }

    string bin = s.substr(zeros, zeros + 1);
    s.erase(0,2*zeros+1);
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

u_int64_t eliasODecode(string &s) {
    u_int64_t n = eliasYDecode(s);
    //k-1,n,x

    string rest=s.substr(0,n-1);
    string bin = "1" + rest;
    s.erase(0,n+1);
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

u_int64_t eliasWDecode(string &s) {
    u_int64_t n = 1;
    size_t pos = 0;

    while (pos < s.size()) {
        if (s[pos] == '0'){
            s.erase(0,pos+1);
            return n;
        }
        u_int64_t chunkLen = n + 1;
        if (pos + chunkLen > s.size()) {
            throw runtime_error("Niepełny kod Eliasza-omega");
        }

        string bin = s.substr(pos, chunkLen);
        try{
            n = stoull(bin, nullptr, 2);
        }catch(exception e){
            e.what();
        }
        
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

u_int64_t fibonacciDecode(string &str) {
    size_t pos = str.find("11");
    string s;
    if (pos == string::npos) {
        s = str;
        str.clear();
    } else {
        s = str.substr(0, pos + 2);    
        str.erase(0, pos + 2);         
    }
    vector<u_int64_t> F = {1, 2};
    while (F.size() < s.size())
        F.push_back(F[F.size() - 1] + F[F.size() - 2]);

    if (s.empty()) throw runtime_error("Niepełny kod fibo");
    u_int64_t value = 0;
    for (size_t i = 0; i < s.size(); ++i) if (s[i] == '1') value += F[i];
    return value - F.back();
}

auto pickCode(char mode){
    switch (mode)
    {
    case 'w':
        return eliasWDecode;
        break;
    case 'y':
        return eliasYDecode;
        break;
    case 'o':
        return eliasODecode;
        break;
    case 'f':
        return fibonacciDecode;
        break;
    default:
        cout<<"Zla metoda kodowania uzywam eliasw"<<endl;
        return eliasWDecode;
        break;
    }
}

void lzwDecode(vector<string> &slownik, ifstream &input, ofstream &output, char mode) {
    u_int64_t (*Method)(string&);
    Method=pickCode(mode);

    char c;
    string bitbuf=bitset<8>(input.get()).to_string()+bitset<8>(input.get()).to_string()+bitset<8>(input.get()).to_string()+bitset<8>(input.get()).to_string();

    
    u_int64_t k1 =Method(bitbuf);

    string w = slownik[k1];
    output << w;

    u_int64_t prev = k1;

    // Główna pętla LZW
    while (input.get(c)) {
        bitbuf=bitbuf+c;
        for(int i=0;i<2;i++){
            if(!input.get(c)){
                break;
            }
            bitbuf=bitbuf+c;
        }
        u_int64_t k;

        // próbujemy wyciągnąć kolejny kod
        k=Method(bitbuf);

        // Jeśli nadal brak pełnego kodu → koniec pliku
        if (bitbuf.empty()) break;

        string entry;

        if (k < slownik.size()) {
            output<<slownik[k];
            slownik.push_back(slownik[prev]+slownik[k][0]);
        } else {
            throw runtime_error("Błąd dekompresji LZW: niepoprawny indeks");
        }

        //output << entry;

        // Dodajemy nowy wpis do słownika (prev + first(entry))
        //slownik.push_back(slownik[prev] + entry[0]);

        prev = k;
    }
}

int main(int argc,char** argv){
    if (argc < 2) {
        cerr << "Using: " << argv[0] << endl;
        return -1;
    }

    string filename = argv[1];
    ifstream input(filename, ios::binary);
    ofstream output("LZWDecoded.bin");
    char mode = input.get();

    vector<string> slownik;

    for (int i = 0; i < 256; ++i) slownik.push_back(string(1, static_cast<char>(i)));
    
    lzwDecode(slownik,input,output,mode);
    
    output.close();
    input.close();

    return 0;
}