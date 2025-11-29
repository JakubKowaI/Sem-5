#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <map>
#include <string>
#include <iomanip>
#include <cstdint>

using namespace std;

#pragma pack(push, 1)
struct TGAHeader {
    uint8_t  idLength;
    uint8_t  colorMapType;
    uint8_t  imageType;
    uint16_t colorMapOrigin;
    uint16_t colorMapLength;
    uint8_t  colorMapDepth;
    uint16_t xOrigin;
    uint16_t yOrigin;
    uint16_t width;
    uint16_t height;
    uint8_t  bitsPerPixel;
    uint8_t  imageDescriptor;
};
#pragma pack(pop)

struct Pixel {
    uint8_t b, g, r;
};

// Function to calculate Shannon Entropy
double calculateEntropy(const vector<uint8_t>& data) {
    if (data.empty()) return 0.0;
    
    map<uint8_t, int> counts;
    for (uint8_t val : data) {
        counts[val]++;
    }

    double entropy = 0.0;
    double total = static_cast<double>(data.size());

    for (auto const& [val, count] : counts) {
        double p = count / total;
        entropy -= p * log2(p);
    }
    return entropy;
}

// Helper to safely get pixel value, treating out of bounds as 0 (black border)
int getVal(const vector<Pixel>& pixels, int x, int y, int width, int height, int channel) {
    if (x < 0 || y < 0 || x >= width || y >= height) return 0;
    const Pixel& p = pixels[y * width + x];
    if (channel == 0) return p.b;
    if (channel == 1) return p.g;
    return p.r;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file.tga>" << endl;
        return 1;
    }

    string filename = argv[1];
    ifstream file(filename, ios::binary);

    if (!file) {
        cerr << "Error: Cannot open file " << filename << endl;
        return 1;
    }

    TGAHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(TGAHeader));

    // Basic validation for uncompressed RGB
    if (header.imageType != 2) {
        cerr << "Error: Only uncompressed True-Color TGA (Type 2) is supported." << endl;
        return 1;
    }
    if (header.bitsPerPixel != 24) {
        cerr << "Error: Only 24-bit depth is supported for this example." << endl;
        return 1;
    }

    // Skip ID field if present
    if (header.idLength > 0) {
        file.seekg(header.idLength, ios::cur);
    }

    int width = header.width;
    int height = header.height;
    size_t numPixels = width * height;

    vector<Pixel> image(numPixels);
    // TGA is usually BGR
    for (size_t i = 0; i < numPixels; ++i) {
        file.read(reinterpret_cast<char*>(&image[i].b), 1);
        file.read(reinterpret_cast<char*>(&image[i].g), 1);
        file.read(reinterpret_cast<char*>(&image[i].r), 1);
    }
    file.close();

    cout << "Processing: " << filename << " (" << width << "x" << height << ")" << endl;
    cout << "------------------------------------------------" << endl;

    // Separate channels for original entropy calculation
    vector<uint8_t> origB, origG, origR, origAll;
    origB.reserve(numPixels); origG.reserve(numPixels); origR.reserve(numPixels); origAll.reserve(numPixels * 3);

    for (const auto& p : image) {
        origB.push_back(p.b);
        origG.push_back(p.g);
        origR.push_back(p.r);
        origAll.push_back(p.b);
        origAll.push_back(p.g);
        origAll.push_back(p.r);
    }

    cout << fixed << setprecision(4);
    cout << "Original Image Entropy:" << endl;
    double eB = calculateEntropy(origB);
    double eG = calculateEntropy(origG);
    double eR = calculateEntropy(origR);
    double eAll = calculateEntropy(origAll);
    
    cout << "  Blue:  " << eB << endl;
    cout << "  Green: " << eG << endl;
    cout << "  Red:   " << eR << endl;
    cout << "  Total: " << eAll << endl;
    cout << "------------------------------------------------" << endl;

    // Predictor logic
    // Using int for calculation to handle negatives before modulo
    // Neighbors: 
    // C B
    // A X
    // X is current at (x,y)
    // A = (x-1, y)
    // B = (x, y-1) -- assuming TGA is top-down or handling coords appropriately. 
    // Note: Standard TGA is bottom-up usually, but relative neighbor logic remains: 
    // "Previous scanline" is y-1 if we iterate y 0..height.
    
    struct Result {
        double entB, entG, entR, entTotal;
    };

    vector<string> methods = {
        "1. X = A",
        "2. X = B",
        "3. X = C",
        "4. X = A + B - C",
        "5. X = A + (B - C)/2",
        "6. X = B + (A - C)/2",
        "7. X = (A + B)/2",
        "8. New JPEG-LS (MED)"
    };

    vector<Result> results;

    for (int m = 0; m < 8; ++m) {
        vector<uint8_t> diffB, diffG, diffR, diffAll;
        diffB.reserve(numPixels); diffG.reserve(numPixels); diffR.reserve(numPixels); diffAll.reserve(numPixels * 3);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                for (int c = 0; c < 3; ++c) { // 0=B, 1=G, 2=R
                    int X = getVal(image, x, y, width, height, c); // Current Actual
                    int A = getVal(image, x - 1, y, width, height, c); // Left
                    int B = getVal(image, x, y - 1, width, height, c); // Up (assuming top-down iter)
                    int C = getVal(image, x - 1, y - 1, width, height, c); // Up-Left

                    // Note on TGA orientation: If TGA header descriptor bit 5 is 0, it's bottom-up.
                    // However, for prediction logic, we just need "previous row" and "previous pixel".
                    // Iterating 0 to height treats memory linearly, so y-1 is the previously processed line.
                    // This is consistent regardless of visual orientation for compression purposes.

                    int pred = 0;
                    if (m == 0) pred = A;
                    else if (m == 1) pred = B;
                    else if (m == 2) pred = C;
                    else if (m == 3) pred = A + B - C;
                    else if (m == 4) pred = A + (B - C) / 2;
                    else if (m == 5) pred = B + (A - C) / 2;
                    else if (m == 6) pred = (A + B) / 2;
                    else if (m == 7) { // JPEG-LS Median Edge Detector
                        if (C >= max(A, B)) pred = min(A, B);
                        else if (C <= min(A, B)) pred = max(A, B);
                        else pred = A + B - C;
                    }

                    int diff = X - pred;
                    // Modulo 256 for 8-bit numbers.
                    // In C++, % of negative numbers is negative. We need mathematical mod.
                    int diff8 = ((diff % 256) + 256) % 256;

                    if (c == 0) diffB.push_back((uint8_t)diff8);
                    else if (c == 1) diffG.push_back((uint8_t)diff8);
                    else diffR.push_back((uint8_t)diff8);
                    
                    diffAll.push_back((uint8_t)diff8);
                }
            }
        }
        results.push_back({
            calculateEntropy(diffB),
            calculateEntropy(diffG),
            calculateEntropy(diffR),
            calculateEntropy(diffAll)
        });
    }

    // Print Results
    double minB = 100, minG = 100, minR = 100, minTot = 100;
    int idxB = -1, idxG = -1, idxR = -1, idxTot = -1;

    for (int i = 0; i < 8; ++i) {
        cout << methods[i] << ":" << endl;
        cout << "  Blue:  " << results[i].entB << endl;
        cout << "  Green: " << results[i].entG << endl;
        cout << "  Red:   " << results[i].entR << endl;
        cout << "  Total: " << results[i].entTotal << endl;
        cout << endl;

        if (results[i].entB < minB) { minB = results[i].entB; idxB = i; }
        if (results[i].entG < minG) { minG = results[i].entG; idxG = i; }
        if (results[i].entR < minR) { minR = results[i].entR; idxR = i; }
        if (results[i].entTotal < minTot) { minTot = results[i].entTotal; idxTot = i; }
    }

    cout << "------------------------------------------------" << endl;
    cout << "Optimal Methods:" << endl;
    cout << "  Best for Blue:  " << methods[idxB] << " (" << minB << ")" << endl;
    cout << "  Best for Green: " << methods[idxG] << " (" << minG << ")" << endl;
    cout << "  Best for Red:   " << methods[idxR] << " (" << minR << ")" << endl;
    cout << "  Best Total:     " << methods[idxTot] << " (" << minTot << ")" << endl;

    return 0;
}