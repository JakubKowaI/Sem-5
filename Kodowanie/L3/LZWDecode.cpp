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
    if(zeros==s.length())throw runtime_error("Niewystarczająca ilość bitów");
    if(2*zeros+1>s.length())throw runtime_error("Niewystarczająca ilość bitów");

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
    string temp=s;
    if(s.length()<9)throw runtime_error("Za mało bitów dla delty");
    u_int64_t n = eliasYDecode(temp);
    //k-1,n,x
    if(temp.length()<n-1) throw runtime_error("Za mało bitów dla delty");

    n = eliasYDecode(s);

    string rest=s.substr(0,n-1);
    //cout<<"N: "<<n<<"\nS: "<<s<<endl;
    string bin = "1" + rest;
    s.erase(0,n-1);
    //cout<<"S after erase: "<<s<<endl;
    //cout<<"BIN: "<<bin<<endl;
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
    while (true) {
        if (pos >= s.size()) {
            throw runtime_error("Niepełny kod Eliasza-omega");
        }
        if (s[pos] == '0') {
            s.erase(0, pos + 1);
            return n;
        }
        u_int64_t chunkLen = n + 1;
        if (pos + chunkLen > s.size()) {
            throw runtime_error("Niepełny kod Eliasza-omega");
        }
        string bin = s.substr(pos, chunkLen);
        try {
            n = stoull(bin, nullptr, 2);
        } catch (const exception &e) {
            throw runtime_error(string("Błąd konwersji w eliasWDecode: ") + e.what());
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
        throw runtime_error("Nie ma 11 w s");
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
        cout<<"Dekoduję metodą Eliasa omega"<<endl;
        return eliasWDecode;
        break;
    case 'y':
        cout<<"Dekoduję metodą Eliasa gamma"<<endl;
        return eliasYDecode;
        break;
    case 'o':
        cout<<"Dekoduję metodą Eliasa delta"<<endl;
        return eliasODecode;
        break;
    case 'f':
        cout<<"Dekoduję metodą Fibonacciego"<<endl;
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

    auto readByteBits = [&](string &bitbuf)->bool{
        char ch;
        if(!input.get(ch)) return false;
        bitset<8> b((unsigned char)ch);
        bitbuf += b.to_string();
        return true;
    };
    
    string bitbuf;
    if(!readByteBits(bitbuf)) throw runtime_error("Pusty lub uszkodzony plik wejściowy");
    u_int64_t k1;
    while (true) {
        try {  u_int64_t val = Method(bitbuf);
            if (val == 0) throw runtime_error("Odczytano 0");
            k1 = val - 1; 
            //cout<<k1<<endl;
            break; }
        catch (const runtime_error &) {
            if(!readByteBits(bitbuf)) throw runtime_error("Niepełny kod na początku pliku");
        }
    }

    string w = slownik.at(k1);
    output << w;
    u_int64_t prev = k1;
    while (true) {
        u_int64_t k;
        try {
            u_int64_t val = Method(bitbuf);
            if (val == 0) throw runtime_error("Odczytano 0"); 
            k = val - 1;
            //readByteBits(bitbuf);
            //cout<<k<<endl;
        } catch (const runtime_error &) {
            if (!readByteBits(bitbuf)) break; 
            else continue;
        }

        if (k < slownik.size()) {
            output << slownik[k];
            //cout<<slownik[k]<<endl;;
            slownik.push_back(slownik[prev] + slownik[k][0]);
        } else if (k == slownik.size()) {
            string entry = slownik[prev] + slownik[prev][0];
            output << entry;
            slownik.push_back(entry);
        } else {
            throw runtime_error("niepoprawny indeks");
        }
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
    ofstream output("LZWDecoded.txt");
    int m = input.get();
    if (m == EOF) { cerr << "Plik wejściowy pusty lub uszkodzony\n"; return -1; }
    char mode = static_cast<char>(m);

    vector<string> slownik;

    for (int i = 0; i < 256; ++i) slownik.push_back(string(1, static_cast<char>(i)));
    
    lzwDecode(slownik,input,output,mode);
    
    output.close();
    input.close();

    return 0;
}