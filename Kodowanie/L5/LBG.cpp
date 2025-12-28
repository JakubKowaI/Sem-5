#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <random>
#include <map>

using namespace std;

#pragma pack(push, 1)
struct Pixel {
    u_int8_t b, g, r;
};
#pragma pack(pop)

#pragma pack(push, 1)
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

struct Color {
    double b, g, r;
};

// double distManhattan(const Color& c1, const Color& c2) {
//     return abs(c1.b - c2.b) + abs(c1.g - c2.g) + abs(c1.r - c2.r);
// }

double distManhattan(const Pixel& p, const Color& c) {
    return abs((double)p.b - c.b) + abs((double)p.g - c.g) + abs((double)p.r - c.r);
}

u_int8_t clamp(double v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return (u_int8_t)v;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file> <bits_per_color>" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];
    int k = stoi(argv[3]);

    if (k < 0 || k > 24) {
        cerr << "k ma byc miedzy 0 a 24." << endl;
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

    Color avg = {0, 0, 0};
    for (const auto& p : image) {
        avg.b += p.b;
        avg.g += p.g;
        avg.r += p.r;
    }
    avg.b /= pixelCount;
    avg.g /= pixelCount;
    avg.r /= pixelCount;

    vector<Color> slownik;
    slownik.push_back(avg);

    int targetSize = 1 << k;
    
    const double Stala = 0.001; 

    while (slownik.size() < targetSize) {
        // dzielimy slownik +- epsilon
        vector<Color> nextslownik;
        nextslownik.reserve(slownik.size() * 2);
        for (const auto& c : slownik) {
            Color c1 = {c.b * (1 + 2*Stala), c.g * ( 1 + Stala), c.r *( 1 + Stala)};
            Color c2 = {c.b * (1 - 2*Stala), c.g * ( 1 - Stala), c.r *( 1 - Stala)};
            nextslownik.push_back(c1);
            nextslownik.push_back(c2);
        }
        slownik = nextslownik;

        double prevD = numeric_limits<double>::max();
        const double LBGEpsilon = 0.01;
        int maxIters = 50;

        for (int iter = 0; iter < maxIters; ++iter) {
            vector<Color> sums(slownik.size(), {0, 0, 0});
            vector<int> counts(slownik.size(), 0);
            double D = 0;

            for (const auto& p : image) {
                int bestIdx = 0;
                double bestDist = numeric_limits<double>::max();
                
                for (size_t i = 0; i < slownik.size(); ++i) {
                    double d = distManhattan(p, slownik[i]);
                    if (d < bestDist) {
                        bestDist = d;
                        bestIdx = i;
                    }
                }
                
                sums[bestIdx].b += p.b;
                sums[bestIdx].g += p.g;
                sums[bestIdx].r += p.r;
                counts[bestIdx]++;
                D += bestDist;
            }

            for (size_t i = 0; i < slownik.size(); ++i) {
                if (counts[i] > 0) {
                    slownik[i].b = sums[i].b / counts[i];
                    slownik[i].g = sums[i].g / counts[i];
                    slownik[i].r = sums[i].r / counts[i];
                } 
            }

            D /= pixelCount;
            if (abs(prevD - D) / prevD < LBGEpsilon) {
                break;
            }
            prevD = D;
        }
    }

    cout<<"Slownik "<<slownik.size()<<endl;

    // for(int i=0;i<10;i++){
    //     cout<<slownik[i].r<<" : "<<slownik[i].g<<" : "<<slownik[i].b<<endl;
    // }

    // for(auto c : slownik){
    //     cout<<c.r<<" : "<<c.g<<" : "<<c.b<<endl;
    // }

    // MSE/SNR
    vector<Pixel> outputImage(pixelCount);
    double mse = 0;
    double signalPower = 0;

    for (int i = 0; i < pixelCount; ++i) {
        const auto& p = image[i];
        int bestIdx = 0;
        double bestDist = numeric_limits<double>::max();

        for (size_t j = 0; j < slownik.size(); ++j) {
            double d = distManhattan(p, slownik[j]);
            if (d < bestDist) {
                bestDist = d;
                bestIdx = j;
            }
        }

        Pixel q;
        q.b = clamp(slownik[bestIdx].b);
        q.g = clamp(slownik[bestIdx].g);
        q.r = clamp(slownik[bestIdx].r);
        outputImage[i] = q;

        // wzor na mse z wikipedii
        double errB = (double)p.b - q.b;
        double errG = (double)p.g - q.g;
        double errR = (double)p.r - q.r;
        mse += errB*errB + errG*errG + errR*errR;

        signalPower += (double)p.b*p.b + (double)p.g*p.g + (double)p.r*p.r;
    }

    mse /= (pixelCount * 3);
    
    double snr = 0;
    if (mse > 0) {
        snr = 10 * log10(signalPower / (mse));
    } else {
        snr = numeric_limits<double>::infinity();
    }

    cout << "MSE: " << mse << endl;
    cout << "SNR: " << snr << " dB" << endl;

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
