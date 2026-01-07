#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include "TGA.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <original_file> <decoded_file>" << endl;
        return 1;
    }

    string file1 = argv[1];
    string file2 = argv[2];

    ifstream in1(file1, ios::binary);
    ifstream in2(file2, ios::binary);

    if (!in1 || !in2) {
        cerr << "Cannot open input files." << endl;
        return 1;
    }

    TGAHeader h1, h2;
    in1.read(reinterpret_cast<char*>(&h1), sizeof(TGAHeader));
    in2.read(reinterpret_cast<char*>(&h2), sizeof(TGAHeader));

    if (h1.width != h2.width || h1.height != h2.height) {
        cerr << "Dimensions do not match." << endl;
        return 1;
    }

    int pixelCount = h1.width * h1.height;
    vector<Pixel> img1(pixelCount), img2(pixelCount);

    in1.read(reinterpret_cast<char*>(img1.data()), pixelCount * sizeof(Pixel));
    in2.read(reinterpret_cast<char*>(img2.data()), pixelCount * sizeof(Pixel));

    double mseR = 0, mseG = 0, mseB = 0;
    double mseTotal = 0;
    double signalPower = 0;

    for (int i = 0; i < pixelCount; ++i) {
        double dr = (double)img1[i].r - img2[i].r;
        double dg = (double)img1[i].g - img2[i].g;
        double db = (double)img1[i].b - img2[i].b;

        mseR += dr * dr;
        mseG += dg * dg;
        mseB += db * db;

        signalPower += (double)img1[i].r * img1[i].r + 
                       (double)img1[i].g * img1[i].g + 
                       (double)img1[i].b * img1[i].b;
    }

    mseR /= pixelCount;
    mseG /= pixelCount;
    mseB /= pixelCount;
    mseTotal = (mseR + mseG + mseB) / 3.0;

    double snr = 0;
    if (mseTotal > 0) {
        snr = 10 * log10((signalPower / (3.0 * pixelCount)) / mseTotal);
    } else {
        snr = 1.0/0.0; // Inf
    }

    cout << "MSE R: " << mseR << endl;
    cout << "MSE G: " << mseG << endl;
    cout << "MSE B: " << mseB << endl;
    cout << "MSE Total: " << mseTotal << endl;
    cout << "SNR: " << snr << " dB" << endl;

    return 0;
}
