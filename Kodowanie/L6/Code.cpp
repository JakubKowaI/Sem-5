#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <string>
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
            nextCodebook.push_back(c * (1.0 + EPSILON));
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

int main(int argc, char** argv) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file> <k>" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];
    int k = stoi(argv[3]);

    if (k < 1 || k > 7) {
        cerr << "k must be between 1 and 7" << endl;
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

    // Separate channels
    vector<double> R, G, B;
    for (const auto& p : image) {
        B.push_back(p.b);
        G.push_back(p.g);
        R.push_back(p.r);
    }

    // Process each channel
    // We need to store codebooks and indices for each channel's L and H bands
    // Structure:
    // Header
    // k (1 byte)
    // Codebooks: 6 * (2^k * sizeof(float))
    // Data: Packed indices

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

    for (int c = 0; c < 3; ++c) {
        vector<double>& pixels = channels[c];
        vector<double> L, H;
        
        // Transform
        for (size_t i = 0; i < pixels.size(); i += 2) {
            double p1 = pixels[i];
            double p2 = (i + 1 < pixels.size()) ? pixels[i+1] : p1;
            
            double l_val = (p1 + p2) / 2.0;
            double h_val = (p1 - p2) / 2.0;
            
            L.push_back(l_val);
            H.push_back(h_val);
        }

        // Prepare data for training LBG
        // For L, we use differences (DPCM)
        vector<double> L_diffs;
        double prevL = 0; // Assuming starting prediction is 0 or 128? Let's use 0.
        // Actually, for training, we can use open loop differences
        // But for better quality, maybe just raw differences
        prevL = 0;
        for (double val : L) {
            L_diffs.push_back(val - prevL);
            prevL = val;
        }

        // Train Codebooks
        vector<double> cb_L = trainLBG(L_diffs, k);
        vector<double> cb_H = trainLBG(H, k);

        codebooks.push_back(cb_L);
        codebooks.push_back(cb_H);

        // Encode
        vector<int> indices_L;
        vector<int> indices_H;

        // DPCM Encoding for L (Closed Loop)
        double reconL = 0;
        for (double val : L) {
            double diff = val - reconL;
            int idx = quantize(diff, cb_L);
            indices_L.push_back(idx);
            reconL += cb_L[idx];
        }

        // Direct Encoding for H
        for (double val : H) {
            int idx = quantize(val, cb_H);
            indices_H.push_back(idx);
        }

        allIndices.push_back(indices_L);
        allIndices.push_back(indices_H);
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
