#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <string>
#include <random>
#include <chrono>
#include <utility>
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

int getVal(const vector<Pixel>& pixels, int x, int y, int width, int height, int channel) {
    // if (x < 0) x = 0;
    // if (y < 0) y = 0;
    // if (x >= width) x = width - 1;
    // if (y >= height) y = height - 1;
    if (x < 0) return 128;
    if (y < 0) return 128;
    if (x >= width) return 128;
    if (y >= height) return 128;
    
    const Pixel& p = pixels[y * width + x];
    if (channel == 0) return p.b;
    if (channel == 1) return p.g;
    return p.r;
}

uint8_t computeLP(const vector<Pixel>& pixels, int x, int y, int width, int height, int channel) {
    int sum = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            sum += getVal(pixels, x + dx, y + dy, width, height, channel);
        }
    }
    return (uint8_t)(sum / 9);
}

static int toS8(uint8_t v) {
    return static_cast<int>(static_cast<int8_t>(v));
}

static uint8_t fromS8(int v) {
    if (v < -128) v = -128;
    if (v > 127) v = 127;
    return static_cast<uint8_t>(static_cast<int8_t>(v));
}

pair<vector<uint8_t>, vector<uint8_t>> LBG(const vector<uint8_t>& data, int k) {
    int targetSize = 1 << k;
    vector<uint8_t> codebook;

    double mean = 0;
    for (uint8_t v : data) mean += toS8(v);
    mean /= data.size();
    codebook.push_back(fromS8((int)lround(mean)));

    mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> dist(1, 15);

    while ((int)codebook.size() < targetSize) {
        vector<uint8_t> newCodebook;
        for (uint8_t c : codebook) {
            int delta = dist(rng);
            int cs = toS8(c);
            newCodebook.push_back(fromS8(cs + dist(rng)));
            newCodebook.push_back(fromS8(cs - dist(rng)));
        }
        codebook = newCodebook;

        double prevDistortion = numeric_limits<double>::max();
        const double convergenceThreshold = 0.001;
        int maxIters = 100;

        for (int iter = 0; iter < maxIters; ++iter) {
            vector<double> sums(codebook.size(), 0);
            vector<int> counts(codebook.size(), 0);
            double distortion = 0;

            for (uint8_t val : data) {
                int bestIdx = 0;
                int bestDist = numeric_limits<int>::max();

                for (size_t i = 0; i < codebook.size(); ++i) {
                    int d = abs(toS8(val) - toS8(codebook[i]));
                    if (d < bestDist) {
                        bestDist = d;
                        bestIdx = (int)i;
                    }
                }

                sums[bestIdx] += toS8(val);
                counts[bestIdx]++;
                distortion += (double)bestDist * (double)bestDist;
            }

            for (size_t i = 0; i < codebook.size(); ++i) {
                if (counts[i] > 0) {
                    codebook[i] = fromS8((int)lround(sums[i] / counts[i]));
                }
            }

            distortion /= data.size();
            if (abs(prevDistortion - distortion) / (prevDistortion + 1e-10) < convergenceThreshold) {
                break;
            }
            prevDistortion = distortion;
        }
    }

    vector<uint8_t> indexes(data.size());
    for (size_t i = 0; i < data.size(); i++) {
        int bestIdx = 0;
        int bestDist = numeric_limits<int>::max();

        for (size_t j = 0; j < codebook.size(); ++j) {
            int d = abs(toS8(data[i]) - toS8(codebook[j]));
            if (d < bestDist) {
                bestDist = d;
                bestIdx = (int)j;
            }
        }
        indexes[i] = (uint8_t)bestIdx;
    }

    return {indexes, codebook};
}

pair<vector<uint8_t>, vector<uint8_t>> LBG_normal(const vector<uint8_t>& data, int k) {
    int targetSize = 1 << k;
    vector<uint8_t> codebook;

    double mean = 0;
    for (uint8_t v : data) mean += v;
    mean /= data.size();
    codebook.push_back((uint8_t)mean);

    mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> dist(1, 15);

    while ((int)codebook.size() < targetSize) {
        vector<uint8_t> newCodebook;
        for (uint8_t c : codebook) {
            int delta = dist(rng);
            int cs = toS8(c);
            newCodebook.push_back(c * (1 + dist(rng)));
            newCodebook.push_back(c * (1 - dist(rng)));
        }
        codebook = newCodebook;

        double prevDistortion = numeric_limits<double>::max();
        const double convergenceThreshold = 0.001;
        int maxIters = 100;

        for (int iter = 0; iter < maxIters; ++iter) {
            vector<double> sums(codebook.size(), 0);
            vector<int> counts(codebook.size(), 0);
            double distortion = 0;

            for (uint8_t val : data) {
                int bestIdx = 0;
                int bestDist = numeric_limits<int>::max();

                for (size_t i = 0; i < codebook.size(); ++i) {
                    int d = abs(val - codebook[i]);
                    if (d < bestDist) {
                        bestDist = d;
                        bestIdx = (int)i;
                    }
                }

                sums[bestIdx] += val;
                counts[bestIdx]++;
                distortion += (double)bestDist * (double)bestDist;
            }

            for (size_t i = 0; i < codebook.size(); ++i) {
                if (counts[i] > 0) {
                    codebook[i] = (uint8_t)(sums[i] / counts[i]);
                }
            }

            distortion /= data.size();
            if (abs(prevDistortion - distortion) / (prevDistortion + 1e-10) < convergenceThreshold) {
                break;
            }
            prevDistortion = distortion;
        }
    }

    vector<uint8_t> indexes(data.size());
    for (size_t i = 0; i < data.size(); i++) {
        int bestIdx = 0;
        int bestDist = numeric_limits<int>::max();

        for (size_t j = 0; j < codebook.size(); ++j) {
            int d = abs(data[i] - codebook[j]);
            if (d < bestDist) {
                bestDist = d;
                bestIdx = (int)j;
            }
        }
        indexes[i] = (uint8_t)bestIdx;
    }

    return {indexes, codebook};
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

    vector<uint8_t> LP[3], HP[3];  // 0=B, 1=G, 2=R
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int ch = 0; ch < 3; ++ch) {
                int original = getVal(image, x, y, width, height, ch);
                uint8_t lp = computeLP(image, x, y, width, height, ch);

                int hpInt = original - (int)lp;
                uint8_t hp = fromS8(hpInt);
                
                LP[ch].push_back(lp);
                HP[ch].push_back(hp);
            }
        }
    }

    vector<uint8_t> LP_diff[3];
    for (int ch = 0; ch < 3; ++ch) {
        int prev = 128;
        LP_diff[ch].reserve(LP[ch].size());
        for (size_t i = 0; i < LP[ch].size(); ++i) {
            // if(!i%width)prev=128;
            int lp = (int)LP[ch][i];
            int diff = lp - prev;
            LP_diff[ch].push_back(fromS8(diff));
            prev = lp;
        }
    }

    vector<uint8_t> indicesLP[3], indicesHP[3];
    vector<uint8_t> codebookLP[3], codebookHP[3];
    for (int ch = 0; ch < 3; ++ch) {
        auto [idxLP, cbLP] = LBG(LP_diff[ch], k);
        // auto [idxLP, cbLP] = LBG_normal(LP[ch], k);
        auto [idxHP, cbHP] = LBG(HP[ch], k);
        indicesLP[ch] = idxLP;
        codebookLP[ch] = cbLP;
        indicesHP[ch] = idxHP;
        codebookHP[ch] = cbHP;
    }

    ofstream output(outputFile, ios::binary);
    if (!output) {
        cerr << "Cannot open output file." << endl;
        return 1;
    }

    output.write(reinterpret_cast<char*>(&header), sizeof(TGAHeader));

    uint8_t k_byte = (uint8_t)k;
    output.write(reinterpret_cast<char*>(&k_byte), 1);

    int codebookSize = 1 << k;

    for (int ch = 0; ch < 3; ++ch) {
        output.write(reinterpret_cast<const char*>(codebookLP[ch].data()), (streamsize)codebookLP[ch].size());
    }
    for (int ch = 0; ch < 3; ++ch) {
        output.write(reinterpret_cast<const char*>(codebookHP[ch].data()), (streamsize)codebookHP[ch].size());
    }

    BitWriter bw(output);
    for (size_t i = 0; i < indicesLP[0].size(); i++) {
        for (int ch = 0; ch < 3; ch++) {
            bw.write(indicesLP[ch][i], k);
            bw.write(indicesHP[ch][i], k);
        }
    }
    bw.flush();

    for(auto c : codebookHP[1]){
        cout<<(int)c<<endl;
    }

    output.close();
    
    cout << "Encoded with k=" << k << endl;
    cout << "Codebook size: " << codebookSize << endl;

    cout<< 120 << " : " << (int)fromS8(120)<<endl;
    cout<< -120 << " : " << (int)fromS8(-120)<<endl;
    cout<< 128 << " : " << (int)fromS8(128)<<endl;
    cout<< -1 << " : " << (int)fromS8(-1)<<endl;

    return 0;
}