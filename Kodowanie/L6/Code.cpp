#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <string>
#include <random>
#include <chrono>
#include "TGA.h"

using namespace std;

struct BitWriter {
    ofstream& out;
    uint8_t buffer;
    int bitsFilled;

    BitWriter(ofstream& o) : out(o), buffer(0), bitsFilled(0) {}

    void write(int value, int bits) {
        for (int i = 0; i < bits; ++i) {
            int bit = (value >> (bits - 1 - i)) & 1;
            buffer = (buffer << 1) | bit;
            bitsFilled++;
            if (bitsFilled == 8) {
                out.put(buffer);
                buffer = 0;
                bitsFilled = 0;
            }
        }
    }

    void flush() {
        if (bitsFilled > 0) {
            buffer = buffer << (8 - bitsFilled);
            out.put(buffer);
            bitsFilled = 0;
            buffer = 0;
        }
    }
};

// LBG Algorithm for 1D scalar quantization
vector<double> trainLBG(const vector<double>& data, int k) {
    int targetSize = 1 << k;
    if (data.empty()) return {0.0};

    // Calculate initial centroid (mean)
    double sum = 0;
    for (double v : data) sum += v;
    double mean = sum / data.size();

    vector<double> codebook;
    codebook.push_back(mean);

    const double EPSILON = 0.001;
    const double DIST_EPSILON = 0.0001;

    while (codebook.size() < targetSize) {
        // Split
        vector<double> nextCodebook;
        for (double c : codebook) {
            nextCodebook.push_back(c * (1.0 + 3*EPSILON));
            nextCodebook.push_back(c * (1.0 - EPSILON));
        }
        codebook = nextCodebook;

        // Iterate (K-means)
        double prevDist = numeric_limits<double>::max();
        
        for (int iter = 0; iter < 100; ++iter) {
            vector<double> sums(codebook.size(), 0.0);
            vector<int> counts(codebook.size(), 0);
            double totalDist = 0;

            for (double val : data) {
                int bestIdx = 0;
                double bestDist = numeric_limits<double>::max();
                for (size_t i = 0; i < codebook.size(); ++i) {
                    double d = abs(val - codebook[i]);
                    if (d < bestDist) {
                        bestDist = d;
                        bestIdx = i;
                    }
                }
                sums[bestIdx] += val;
                counts[bestIdx]++;
                totalDist += bestDist;
            }

            for (size_t i = 0; i < codebook.size(); ++i) {
                if (counts[i] > 0) {
                    codebook[i] = sums[i] / counts[i];
                }
            }

            double avgDist = totalDist / data.size();
            if (abs(prevDist - avgDist) / (prevDist + 1e-9) < DIST_EPSILON) {
                break;
            }
            prevDist = avgDist;
        }
    }
    return codebook;
}

int quantize(double value, const vector<double>& codebook) {
    int bestIdx = 0;
    double bestDist = numeric_limits<double>::max();
    for (size_t i = 0; i < codebook.size(); ++i) {
        double d = abs(value - codebook[i]);
        if (d < bestDist) {
            bestDist = d;
            bestIdx = i;
        }
    }
    return bestIdx;
}

u_int8_t new1(u_int8_t w, u_int8_t n, u_int8_t nw){
    int neww=w;
    int newn=n;
    int newnw=nw;
    if(newnw>=max(newn,neww))return min(newn,neww);
    if(newnw<=min(newn,neww))return max(newn,neww);
    return neww+newn-newnw;//Na wykładzie był błąd w zapisie
}

int getVal(const vector<Pixel>& pixels, int x, int y, int width, int height, int channel) {
    if (x < 0 || y < 0 || x >= width || y >= height) return 0;
    const Pixel& p = pixels[y * width + x];
    if (channel == 0) return p.b;
    if (channel == 1) return p.g;
    return p.r;
}

u_int8_t clamp(double v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return (u_int8_t)v;
}

vector<u_int8_t> LBG(vector<double> H,int k){
    vector<double> slownik;
    mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());
    slownik.push_back((u_int8_t)(rng()%256));

    int targetSize = 1 << k;
    
    const double Stala = 0.01; 

    while (slownik.size() < targetSize) {
        // dzielimy slownik +- epsilon
        vector<double> nextslownik;
        nextslownik.reserve(slownik.size() * 2);
        for (const auto& c : slownik) {
            nextslownik.push_back(c * (1 + 3*Stala));
            nextslownik.push_back(c * (1 - Stala));
        }
        slownik = nextslownik;

        double prevD = numeric_limits<double>::max();
        const double LBGEpsilon = 0.01;
        int maxIters = 50;

        for (int iter = 0; iter < maxIters; ++iter) {
            vector<int> sums(slownik.size(), 0);
            vector<int> counts(slownik.size(), 0);
            double D = 0;

            for (const auto& h : H) {
                int bestIdx = 0;
                int bestDist = numeric_limits<int>::max();
                
                for (size_t i = 0; i < slownik.size(); ++i) {
                    int d = abs(h - slownik[i]);
                    if (d < bestDist) {
                        bestDist = d;
                        bestIdx = i;
                    }
                }
                
                sums[bestIdx] += h;
                counts[bestIdx]++;
                D += bestDist;
            }

            for (size_t i = 0; i < slownik.size(); ++i) {
                if (counts[i] > 0) {
                    slownik[i] = sums[i] / counts[i];
                } 
            }

            D /= H.size();
            if (abs(prevD - D) / prevD < LBGEpsilon) {
                break;
            }
            prevD = D;
        }
    }
   
    vector<u_int8_t> output(H.size(),0);
 

    for (int i =0;i<H.size();i++) {
        int bestIdx = 0;
        double bestDist = numeric_limits<double>::max();

        for (size_t j = 0; j < slownik.size(); ++j) {
            double d = abs(H[i] - slownik[j]);
            if (d < bestDist) {
                bestDist = d;
                bestIdx = j;
            }
        }
        
        output[i]= clamp(slownik[bestIdx]);
    }
    return output;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file> <k>" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];
    int k = stoi(argv[3]);

    if (k < 1 || k > 7) {
        cerr << "k ma byc miedzy 1 a 7" << endl;
        return 1;
    }

    ifstream input(inputFile, ios::binary);
    if (!input) {
        cerr << "Cannot open input file." << endl;
        return 1;
    }

    TGAHeader header;
    input.read(reinterpret_cast<char*>(&header), sizeof(TGAHeader));

    int width = header.width;
    int height = header.height;
    int pixelCount = width * height;

    vector<Pixel> image(pixelCount);
    input.read(reinterpret_cast<char*>(image.data()), pixelCount * sizeof(Pixel));
    input.close();

    vector<double> R, G, B;
    for (const auto& p : image) {
        R.push_back(p.r);
        G.push_back(p.g);
        B.push_back(p.b);
    }

    ofstream output(outputFile, ios::binary);
    if (!output) {
        cerr << "Cannot open output file." << endl;
        return 1;
    }

    output.write(reinterpret_cast<char*>(&header), sizeof(TGAHeader));
    uint8_t k_byte = (uint8_t)k;
    output.write(reinterpret_cast<char*>(&k_byte), 1);

    vector<vector<double>> channels = {B, G, R}; // Order B, G, R
    vector<vector<double>> codebooks;
    vector<vector<int>> allIndices;
    vector<vector<double>> L,H;
    vector<vector<u_int8_t>> outputL,outputH;
    
    for (int c = 0; c < 3; ++c) {
        //vector<double>& pixels = channels[c];
        //vector<u_int8_t> L, H;
        double prev=255;    
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                for (int c = 0; c < 3; ++c) { // 0=B, 1=G, 2=R
                    int X = getVal(image, x, y, width, height, c);
                    int w = getVal(image, x - 1, y, width, height, c); // Gora
                    int n = getVal(image, x, y - 1, width, height, c); // Lewo
                    int nw = getVal(image, x - 1, y - 1, width, height, c); // Skos

                    double pred = (w+n+nw)/3;
                    double diff = prev - pred;
                    prev=pred;
                    double HP = X - pred;
                    L[c].push_back(diff);
                    H[c].push_back(HP);
                }
            }
        }

        outputH[0]=LBG(H[0],k);
        outputH[1]=LBG(H[1],k);
        outputH[2]=LBG(H[2],k);
        for(int i=0;i<3;i++){
            for(auto l : L[i]){
                outputL[i].push_back(clamp(l));
            }
        }
    }

    // Write Codebooks
    for (const auto& cb : codebooks) {
        for (double val : cb) {
            float f = (float)val;
            output.write(reinterpret_cast<char*>(&f), sizeof(float));
        }
    }

    // Write Indices
    BitWriter bw(output);
    // Order: B_L, B_H, G_L, G_H, R_L, R_H
    // We can interleave or write sequentially. Sequential is easier.
    for (const auto& indices : allIndices) {
        for (int idx : indices) {
            bw.write(idx, k);
        }
    }
    bw.flush();

    output.close();
    
    // Calculate stats for user info (optional, but good for debugging)
    cout << "Encoded with k=" << k << endl;
    cout << "Codebook size: " << (1 << k) << endl;

    return 0;
}
