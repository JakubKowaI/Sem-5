#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

void przesun(int c,vector<u_int8_t> &v,const vector<u_int8_t> data){
    if(c==0) return;
    if (c > v.size()) c = v.size();
    for(int i=0;i+c<v.size();i++){
        v[i]=v[i+c];
    }
    size_t start = v.size() - c;
    for(size_t i = 0; i < data.size() && (start + i) < v.size(); ++i){
        v[start + i] = data[i];
    }
}

pair<u_int8_t,u_int8_t> znajdzPrefix(vector<u_int8_t> slow,vector<u_int8_t> buf){
    pair<u_int8_t,u_int8_t> pc;

    auto res = search(begin(slow), end(slow), begin(buf), end(buf));
    if(res == end(slow)) {
        pc=pair<u_int8_t,u_int8_t>(0,0);
    } else {
        pc=pair<u_int8_t,u_int8_t>(distance(begin(slow),res),buf.size());
    }

    return pc;
}

int main(int argc,char** argv){
    if (argc < 2) {
        cerr << "Using: " << argv[0] << " <encoded-file>" << endl;
        return -1;
    }

    string filename = argv[1];
    ifstream input(filename, ios::binary);
    ofstream output("LZ77Decoded.txt", ios::binary);

    if (!input.is_open()) {
        cerr << "File not found: " << filename << endl;
        return -1;
    }

    const size_t ssize = 255;

    // wczytaj pierwszy bajt (nagłówek) — tak jak enkoder zapisywał
    int r = input.get();
    if (r == EOF) return 0;
    u_int8_t first = static_cast<u_int8_t>(r);
    output.put(static_cast<char>(first));

    // słownik inicjalny
    vector<u_int8_t> slownik(ssize, 0);
    fill(slownik.begin(), slownik.end(), first);

    // czytamy pary bajtów: (offset, len) lub (0, literal)
    while (true) {
        int a = input.get();
        if (a == EOF) break;
        int b = input.get();
        if (b == EOF) break;

        u_int8_t off = static_cast<u_int8_t>(a);
        u_int8_t len = static_cast<u_int8_t>(b);

        if (off == 0) {
            // literal: drugi bajt to wartość do zapisania
            u_int8_t lit = len; // b zawiera literal
            output.put(static_cast<char>(lit));
            przesun(1, slownik, vector<u_int8_t>{lit});
        } else {
            // dopasowanie: off = pozycja w słowniku, len = długość
            size_t pos = static_cast<size_t>(off);
            size_t count = static_cast<size_t>(len);
            // zabezpieczenie: nie wychodzimy poza słownik
            if (pos >= slownik.size()) {
                // nieprawidłowy offset — przerywamy, można też zgłosić błąd
                break;
            }
            if (pos + count > slownik.size()) {
                count = slownik.size() - pos;
            }
            vector<u_int8_t> toAdd;
            toAdd.reserve(count);
            for (size_t i = 0; i < count; ++i) {
                u_int8_t byte = slownik[pos + i];
                output.put(static_cast<char>(byte));
                toAdd.push_back(byte);
            }
            przesun(count, slownik, toAdd);
        }
    }

    output.close();
    input.close();
    return 0;
}