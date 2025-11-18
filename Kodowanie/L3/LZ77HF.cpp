#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>

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

struct HNode {
    u_int32_t freq;
    int16_t symbol;
    HNode *l, *r;
    HNode(u_int32_t f, int16_t s=-1): freq(f), symbol(s), l(nullptr), r(nullptr) {}
};

struct Cmp {
    bool operator()(HNode* a, HNode* b) const { return a->freq > b->freq; }
};

void buildCodes(HNode* node, vector<vector<bool>>& codes, vector<bool>& cur) {
    if(!node) return;
    if(node->symbol >= 0) {
        codes[(u_int8_t)node->symbol] = cur;
        return;
    }
    cur.push_back(false);
    buildCodes(node->l, codes, cur);
    cur.back() = true;
    buildCodes(node->r, codes, cur);
    cur.pop_back();
}

void serializeTree(HNode* node, vector<u_int8_t>& outBits, vector<u_int8_t>& outVals) {
    if(!node) return;
    if(node->symbol >= 0) {
        outBits.push_back(1);
        outVals.push_back((u_int8_t)node->symbol);
    } else {
        outBits.push_back(0);
        serializeTree(node->l, outBits, outVals);
        serializeTree(node->r, outBits, outVals);
    }
}

void freeTree(HNode* node) {
    if(!node) return;
    freeTree(node->l);
    freeTree(node->r);
    delete node;
}

// BitWriter prosty
struct BitWriter {
    ofstream &out;
    u_int8_t buf = 0;
    int bcount = 0;
    BitWriter(ofstream &o): out(o) {}
    void writeBit(bool bit) {
        buf = (buf << 1) | (bit ? 1 : 0);
        if(++bcount == 8) { out.put((char)buf); buf = 0; bcount = 0; }
    }
    void writeBitsFromVector(const vector<bool>& bits) {
        for(bool b: bits) writeBit(b);
    }
    void writeByte(u_int8_t v) {
        if(bcount == 0) { out.put((char)v); return; }
        for(int i=7;i>=0;--i) writeBit((v >> i) & 1);
    }
    void flush() {
        if(bcount>0) {
            buf <<= (8 - bcount);
            out.put((char)buf);
            buf = 0; bcount = 0;
        }
    }
};

// flush jednego bloku tokenów: buduj Huffmana i zapisuj (nagłówek + dane bitowe)
bool flushBlock(ofstream &output, vector<u_int8_t> &tokens) {
    if(tokens.empty()) return true;
    // częstotliwości
    vector<u_int32_t> freq(256,0);
    for(u_int8_t b: tokens) freq[b]++;

    // buduj drzewo
    priority_queue<HNode*, vector<HNode*>, Cmp> pq;
    for(int i=0;i<256;i++){
        if(freq[i]>0) pq.push(new HNode(freq[i], i));
    }
    if(pq.empty()) return false;

    while(pq.size()>1){
        HNode* a = pq.top(); pq.pop();
        HNode* b = pq.top(); pq.pop();
        HNode* parent = new HNode(a->freq + b->freq, -1);
        parent->l = a; parent->r = b;
        pq.push(parent);
    }
    HNode* root = pq.top();

    // generuj kody
    vector<vector<bool>> codes(256);
    vector<bool> cur;
    buildCodes(root, codes, cur);

    // serializuj drzewo (preorder marker)
    vector<u_int8_t> treeBits;
    vector<u_int8_t> treeVals;
    serializeTree(root, treeBits, treeVals);

    // zapisz nagłówek bloku:
    // [4 bytes: treeBits.size()] [4 bytes: treeVals.size()] [treeBits as bits padded] [treeVals bytes]
    auto write_u32 = [&](u_int32_t v){
        output.put((char)(v & 0xFF));
        output.put((char)((v>>8) & 0xFF));
        output.put((char)((v>>16) & 0xFF));
        output.put((char)((v>>24) & 0xFF));
    };
    write_u32((u_int32_t)treeBits.size());
    write_u32((u_int32_t)treeVals.size());

    // zapisz treeBits bitowo
    {
        BitWriter bw(output);
        for(u_int8_t mb : treeBits) bw.writeBit(mb ? 1 : 0);
        bw.flush();
    }
    // zapisz treeVals
    for(u_int8_t v : treeVals) output.put((char)v);

    // zakoduj tokens używając kodów i zapisz bitowo
    {
        BitWriter bw(output);
        for(u_int8_t b : tokens) {
            const vector<bool>& code = codes[b];
            bw.writeBitsFromVector(code);
        }
        bw.flush();
    }

    freeTree(root);
    tokens.clear();
    return true;
}

int main(int argc,char** argv){
    if (argc < 2) {
        cerr << "Using: " << argv[0] << endl;
        return -1;
    }

    string filename = argv[1];
    ifstream input(filename, ios::binary);
    ofstream output("LZ77Coded.bin", ios::binary);

    if (!input.is_open()) {
        cerr << "File not found: " << endl;
        return -1;
    }
    int ssize=255;
    int wsize=256;
    const size_t BLOCK = 4096; // liczba tokenów pomiędzy rebuildami Huffmana

    u_int8_t first = input.get();
    if (first == EOF) return 0;
    // zapisujemy pierwszy bajt surowy (jak wcześniej)
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

    // bufor tokenów (emitujemy do niego zamiast do pliku)
    vector<u_int8_t> tokenBuf;
    tokenBuf.reserve(BLOCK);

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

        if(maxc==0){
            tokenBuf.push_back(0);
            tokenBuf.push_back(wejscie[0]);

            przesun(1, slownik, vector<u_int8_t>{wejscie[0]});

            wejscie.erase(wejscie.begin());
            int v = input.get();
            if (v != EOF) wejscie.push_back(static_cast<u_int8_t>(v));
        }else{
            tokenBuf.push_back(p);
            tokenBuf.push_back(maxc);

            vector<u_int8_t> toAdd(wejscie.begin(), wejscie.begin() + maxc);
            przesun(maxc, slownik, toAdd);

            wejscie.erase(wejscie.begin(), wejscie.begin() + maxc);
            for (size_t i = 0; i < maxc && wejscie.size() < wsize; ++i) {
                int v = input.get();
                if (v == EOF) break;
                wejscie.push_back(static_cast<u_int8_t>(v));
            }
        }

        // jeżeli bufor osiągnął rozmiar bloku, zapisz go (rebuilt Huffman)
        if(tokenBuf.size() >= BLOCK) {
            if(!flushBlock(output, tokenBuf)) {
                cerr << "Flush block failed" << endl;
                return -1;
            }
        }
    }

    // flush ostatniego bloku
    if(!tokenBuf.empty()) {
        if(!flushBlock(output, tokenBuf)) {
            cerr << "Final flush failed" << endl;
            return -1;
        }
    }

    output.close();
    input.close();
    return 0;
}