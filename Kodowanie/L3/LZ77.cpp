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
        cerr << "Using: " << argv[0] << endl;
        return -1;
    }

    string filename = argv[1];
    ifstream input(filename, ios::binary);
    // ifstream input("/home/kuba/Projects/Sem-5/Kodowanie/listy/testy1/pan-tadeusz-czyli-ostatni-zajazd-na-litwie.txt", ios::binary);
    ofstream output("LZ77Coded.bin");

    if (!input.is_open()) {
        cerr << "File not found: " << endl;
        return -1;
    }
    int ssize=255;
    int wsize=256;

    u_int8_t first = input.get();
    if (first == EOF) return 0;
    output.put(static_cast<char>(first));

    vector<u_int8_t> slownik(ssize,0);
    fill(slownik.begin(),slownik.end(),first);
    vector<u_int8_t> wejscie;
    wejscie.reserve(wsize);
    
    while (wejscie.size() < wsize) {
        int v = input.get();
        if (v == EOF) break;
        wejscie.push_back(static_cast<u_int8_t>(v));
    }

    while (!wejscie.empty()) {
        u_int8_t maxc=0;
        u_int8_t p=0;
        for(int i=0;i<wejscie.size();i++){
            vector<u_int8_t> temp(wejscie.begin(),wejscie.begin()+i);
            pair<u_int8_t,u_int8_t> pc= znajdzPrefix(slownik,temp);
            if(get<1>(pc)>=maxc){
                maxc=get<1>(pc);
                p=get<0>(pc);
            }else{
                break;
            }
        }
        //cout<<"Test"<<endl; 
        if(maxc==0){
            output.put(static_cast<char>(0));
            output.put(static_cast<char>(wejscie[0]));

            przesun(1, slownik, vector<u_int8_t>{wejscie[0]});

            wejscie.erase(wejscie.begin());
            int v = input.get();
            if (v != EOF) wejscie.push_back(static_cast<u_int8_t>(v));
        }else{
            output.put(static_cast<char>(p));
            output.put(static_cast<char>(maxc));

            // dodaj do słownika maksymalny prefix
            vector<u_int8_t> toAdd(wejscie.begin(), wejscie.begin() + maxc);
            przesun(maxc, slownik, toAdd);

            // usuń z wejscie maxc bajtów i dopełnij z pliku
            wejscie.erase(wejscie.begin(), wejscie.begin() + maxc);
            for (size_t i = 0; i < maxc && wejscie.size() < wsize; ++i) {
                int v = input.get();
                if (v == EOF) break;
                wejscie.push_back(static_cast<u_int8_t>(v));
            }
        }
    }

    output.close();
    input.close();

    return 0;
}