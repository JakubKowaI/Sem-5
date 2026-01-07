#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include "TGA.h"

using namespace std;

struct BitReader {
    ifstream& in;
    uint8_t buffer;
    int bitsRemaining;

    BitReader(ifstream& i) : in(i), buffer(0), bitsRemaining(0) {}

    int read(int bits) {
        int value = 0;
        for (int i = 0; i < bits; ++i) {
            if (bitsRemaining == 0) {
                char c;
                if (!in.get(c)) return value; // Should not happen if file is correct
                buffer = (uint8_t)c;
                bitsRemaining = 8;
            }
            int bit = (buffer >> (bitsRemaining - 1)) & 1;
            value = (value << 1) | bit;
            bitsRemaining--;
        }
        return value;
    }
};

uint8_t clamp(double v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return (uint8_t)v;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];

    ifstream input(inputFile, ios::binary);
    if (!input) {
        cerr << "Cannot open input file." << endl;
        return 1;
    }

    TGAHeader header;
    input.read(reinterpret_cast<char*>(&header), sizeof(TGAHeader));

    uint8_t k_byte;
    input.read(reinterpret_cast<char*>(&k_byte), 1);
    int k = (int)k_byte;

    int codebookSize = 1 << k;
    vector<vector<double>> codebooks(6); // B_L, B_H, G_L, G_H, R_L, R_H

    for (int i = 0; i < 6; ++i) {
        codebooks[i].resize(codebookSize);
        for (int j = 0; j < codebookSize; ++j) {
            float f;
            input.read(reinterpret_cast<char*>(&f), sizeof(float));
            codebooks[i][j] = (double)f;
        }
    }

    int width = header.width;
    int height = header.height;
    int pixelCount = width * height;
    int numPairs = (pixelCount + 1) / 2;

    BitReader br(input);

    vector<vector<double>> reconstructedChannels(3); // B, G, R

    for (int c = 0; c < 3; ++c) {
        int cbIdx_L = c * 2;
        int cbIdx_H = c * 2 + 1;
        
        vector<double> L(numPairs);
        vector<double> H(numPairs);

        // Read L indices and reconstruct L (DPCM)
        double reconL = 0;
        for (int i = 0; i < numPairs; ++i) {
            int idx = br.read(k);
            double diff = codebooks[cbIdx_L][idx];
            reconL += diff;
            L[i] = reconL;
        }

        // Read H indices and reconstruct H
        for (int i = 0; i < numPairs; ++i) {
            int idx = br.read(k);
            H[i] = codebooks[cbIdx_H][idx];
        }

        // Inverse Transform
        vector<double>& pixels = reconstructedChannels[c];
        pixels.reserve(pixelCount);
        
        for (int i = 0; i < numPairs; ++i) {
            double l_val = L[i];
            double h_val = H[i];
            
            double p1 = l_val + h_val;
            double p2 = l_val - h_val;
            
            pixels.push_back(p1);
            if (pixels.size() < (size_t)pixelCount) {
                pixels.push_back(p2);
            }
        }
    }

    input.close();

    // Merge channels into pixels
    vector<Pixel> outputImage(pixelCount);
    for (int i = 0; i < pixelCount; ++i) {
        outputImage[i].b = clamp(reconstructedChannels[0][i]);
        outputImage[i].g = clamp(reconstructedChannels[1][i]);
        outputImage[i].r = clamp(reconstructedChannels[2][i]);
    }

    ofstream output(outputFile, ios::binary);
    if (!output) {
        cerr << "Cannot open output file." << endl;
        return 1;
    }

    output.write(reinterpret_cast<char*>(&header), sizeof(TGAHeader));
    output.write(reinterpret_cast<char*>(outputImage.data()), pixelCount * sizeof(Pixel));
    output.close();

    return 0;
}
