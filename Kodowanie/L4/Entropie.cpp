#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <bitset>
#include <unordered_map>
#include <cmath>
#include <map>

using namespace std;
using Predictor = u_int8_t(*)(u_int8_t,u_int8_t,u_int8_t);

#pragma pack(push, 1)
struct Pixel {
    u_int8_t b, g, r;
};
#pragma pack(pop)
static_assert(sizeof(Pixel) == 3, "Pixel must be 3 bytes");

#pragma pack(push,1)
struct TGAHeader {
    u_int8_t  idLength;
    u_int8_t  colorMapType;
    u_int8_t  imageType;
    u_int16_t colorMapOrigin;
    u_int16_t colorMapLength;
    u_int8_t  colorMapDepth;
    u_int16_t xOrigin;
    u_int16_t yOrigin;
    u_int16_t width;
    u_int16_t height;
    u_int8_t  bitsPerPixel;
    u_int8_t  imageDescriptor;
};
#pragma pack(pop)

pair<int,int> getHeader(ifstream &input){
    TGAHeader header;
    input.read(reinterpret_cast<char*>(&header), sizeof(TGAHeader));
    return pair<int,int>(header.height,header.width);
}

Pixel getPixel(ifstream &input){
    Pixel p;
    input.read(reinterpret_cast<char*>(&p), sizeof(Pixel));
    return p;
}

double getEntropy(const vector<u_int8_t>& data) {
    if (data.empty()) return 0.0;
    
    map<u_int8_t, int> counts;
    for (u_int8_t val : data) {
        counts[val]++;
    }

    double entropy = 0.0;
    double total = data.size();

    for (auto const& [val, count] : counts) {
        double p = (double)count / total;
        entropy -= p * log2(p);
    }
    return entropy;
}

int getVal(const vector<Pixel>& pixels, int x, int y, int width, int height, int channel) {
    if (x < 0 || y < 0 || x >= width || y >= height) return 0;
    const Pixel& p = pixels[y * width + x];
    if (channel == 0) return p.b;
    if (channel == 1) return p.g;
    return p.r;
}

u_int8_t old1(u_int8_t w, u_int8_t n, u_int8_t nw){
    int neww=w;
    int newn=n;
    int newnw=nw;
    return neww;
}

u_int8_t old2(u_int8_t w, u_int8_t n, u_int8_t nw){
    int neww=w;
    int newn=n;
    int newnw=nw;
    return newn;
}

u_int8_t old3(u_int8_t w, u_int8_t n, u_int8_t nw){
    int neww=w;
    int newn=n;
    int newnw=nw;
    return newnw;
}

u_int8_t old4(u_int8_t w, u_int8_t n, u_int8_t nw){
    int neww=w;
    int newn=n;
    int newnw=nw;
    return newn+neww-newnw;
}

u_int8_t old5(u_int8_t w, u_int8_t n, u_int8_t nw){
    int neww=w;
    int newn=n;
    int newnw=nw;
    return newn+(neww-newnw)/2;
}

u_int8_t old6(u_int8_t w, u_int8_t n, u_int8_t nw){
    int neww=w;
    int newn=n;
    int newnw=nw;
    return neww+(newn-newnw)/2;
}

u_int8_t old7(u_int8_t w, u_int8_t n, u_int8_t nw){
    int neww=w;
    int newn=n;
    int newnw=nw;
    return (newn+neww)/2;
}

u_int8_t new1(u_int8_t w, u_int8_t n, u_int8_t nw){
    int neww=w;
    int newn=n;
    int newnw=nw;
    if(newnw>=max(newn,neww))return min(newn,neww);
    if(newnw<=min(newn,neww))return max(newn,neww);
    return neww+newn-newnw;//Na wykładzie był błąd w zapisie
}

Predictor cycleMethod(int mode){
    if(mode<0||mode>7)return nullptr;
    static constexpr Predictor methods[]={old1,old2,old3,old4,old5,old6,old7,new1};
    return methods[mode];
}

int main(int argc,char** argv){
    if (argc < 2) {
        cerr << "Using: " << argv[0] << endl;
        return -1;
    }

    ifstream input(argv[1],ios::binary);
    pair<int,int> hxw=getHeader(input);
    int pixelCount=get<0>(hxw)*get<1>(hxw);

    vector<Pixel> image(pixelCount);

    for(int i=0;i<pixelCount;i++){
        image[i]=getPixel(input);
    }
    input.close();
    vector<u_int8_t> r;
    r.reserve(pixelCount);
    vector<u_int8_t> g;
    g.reserve(pixelCount);
    vector<u_int8_t> b;
    b.reserve(pixelCount);
    vector<u_int8_t> whole;
    whole.reserve(pixelCount*3);
    for (const auto& p : image) {
        b.push_back(p.b);
        g.push_back(p.g);
        r.push_back(p.r);
        whole.push_back(p.b);
        whole.push_back(p.g);
        whole.push_back(p.r);
    }
    cout<<"Original image entropy: \nRed: "<<getEntropy(r)<<"\nGreen: "<<getEntropy(g)<<"\nBlue: "<<getEntropy(b)<<"\nWhole: "<<getEntropy(whole)<<endl<<endl;
    double minr=999.0;
    double ming=999.0;
    double minb=999.0;
    double minwhole=999.0;
    int minR,minG,minB,minWhole;
    for(int i=0;i<8;i++){
        auto predictor = cycleMethod(i);
        vector<u_int8_t> tempb, tempg, tempr, tempwhole;

        for (int y = 0; y < get<0>(hxw); y++) {
            for (int x = 0; x < get<1>(hxw); x++) {
                for (int c = 0; c < 3; ++c) { // 0=B, 1=G, 2=R
                    int X = getVal(image, x, y, get<1>(hxw), get<0>(hxw), c);
                    int w = getVal(image, x - 1, y, get<1>(hxw), get<0>(hxw), c); // Gora
                    int n = getVal(image, x, y - 1, get<1>(hxw), get<0>(hxw), c); // Lewo
                    int nw = getVal(image, x - 1, y - 1, get<1>(hxw), get<0>(hxw), c); // Skos

                    u_int8_t pred = predictor(w,n,nw);

                    int diff = X - pred;
                    diff =((diff % 256) + 256) % 256;

                    if (c == 0) tempb.push_back((u_int8_t)diff);
                    else if (c == 1) tempg.push_back((u_int8_t)diff);
                    else tempr.push_back((u_int8_t)diff);
                    tempwhole.push_back((u_int8_t)diff);
                }
            }
        }
        double tempEr=getEntropy(tempr);
        double tempEg=getEntropy(tempg);
        double tempEb=getEntropy(tempb);
        double tempEwhole=getEntropy(tempwhole);
        if(tempEr<minr){
            minr=tempEr;
            minR=i+1;
        }
        if(tempEg<ming){
            ming=tempEg;
            minG=i+1;
        }
        if(tempEb<minb){
            minb=tempEb;
            minB=i+1;
        }
        if(tempEwhole<minwhole){
            minwhole=tempEwhole;
            minWhole=i+1;
        }
        cout<<"Image entropy for "<<i+1<<" function (8 means new JPEG-LS): \nRed: "<<tempEr<<"\nGreen: "<<tempEg<<"\nBlue: "<<tempEb<<"\nWhole: "<<tempEwhole<<endl<<endl;
    }
    cout<<"Best Entropies: "<<"\nRed: "<<minr<<" dla "<<minR<<"\nGreen: "<<ming<<" dla "<<minG<<"\nBlue: "<<minb<<" dla "<<minB<<"\nWhole: "<<minwhole<<" dla "<<minWhole<<endl;

    

    return 0;
}