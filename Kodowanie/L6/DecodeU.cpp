
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include "TGA.h"

using namespace std;

struct BitReader {
	ifstream& in;
	uint8_t buffer;
	int bitsRemaining;

	BitReader(ifstream& i) : in(i), buffer(0), bitsRemaining(0) {}

	bool readBits(int bits, int& outValue) {
		int value = 0;
		for (int i = 0; i < bits; ++i) {
			if (bitsRemaining == 0) {
				char c;
				if (!in.get(c)) {
					outValue = value;
					return false;
				}
				buffer = static_cast<uint8_t>(c);
				bitsRemaining = 8;
			}
			int bit = (buffer >> (bitsRemaining - 1)) & 1;
			value = (value << 1) | bit;
			bitsRemaining--;
		}
		outValue = value;
		return true;
	}
};

static int toS8(uint8_t v) {
	return static_cast<int>(static_cast<int8_t>(v));
}

static uint8_t clampToU8(int v) {
	if (v < 0) return 0;
	if (v > 255) return 255;
	return static_cast<uint8_t>(v);
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
	if (!input) {
		cerr << "Failed to read TGA header." << endl;
		return 1;
	}

	uint8_t k_byte;
	input.read(reinterpret_cast<char*>(&k_byte), 1);
	if (!input) {
		cerr << "Failed to read k." << endl;
		return 1;
	}
	int k = static_cast<int>(k_byte);
	if (k < 1 || k > 7) {
		cerr << "Invalid k in file (expected 1..7)." << endl;
		return 1;
	}

	int width = header.width;
	int height = header.height;
	if (width <= 0 || height <= 0) {
		cerr << "Invalid image dimensions." << endl;
		return 1;
	}

	int codebookSize = 1 << k;
	int pixelCount = width * height;

	vector<uint8_t> codebookLP[3], codebookHP[3];
	for (int ch = 0; ch < 3; ++ch) {
		codebookLP[ch].resize(codebookSize);
		input.read(reinterpret_cast<char*>(codebookLP[ch].data()), (streamsize)codebookLP[ch].size());
		if (!input) {
			cerr << "Unexpected EOF while reading LP codebook." << endl;
			return 1;
		}
	}
	for (int ch = 0; ch < 3; ++ch) {
		codebookHP[ch].resize(codebookSize);
		input.read(reinterpret_cast<char*>(codebookHP[ch].data()), (streamsize)codebookHP[ch].size());
		if (!input) {
			cerr << "Unexpected EOF while reading HP codebook." << endl;
			return 1;
		}
	}

	BitReader br(input);
	vector<Pixel> outputImage(pixelCount);
	int prevLP[3] = {128, 128, 128};

	for (int i = 0; i < pixelCount; ++i) {
		int rgbI[3];
		for (int ch = 0; ch < 3; ++ch) {
			int idxLP = 0;
			int idxHP = 0;
			if (!br.readBits(k, idxLP) || !br.readBits(k, idxHP)) {
				cerr << "Unexpected EOF while reading indices." << endl;
				return 1;
			}
			if (idxLP < 0 || idxLP >= codebookSize || idxHP < 0 || idxHP >= codebookSize) {
				cerr << "Index out of range." << endl;
				return 1;
			}

			int diff = toS8(codebookLP[ch][idxLP]);
			int hp = toS8(codebookHP[ch][idxHP]);
            // if(!i%width)prevLP[ch]=128;
			int lp = prevLP[ch] + diff;
			lp = (int)clampToU8(lp);
			prevLP[ch] = lp;
			rgbI[ch] = lp + hp;
		}

		outputImage[i].b = clampToU8(rgbI[0]);
		outputImage[i].g = clampToU8(rgbI[1]);
		outputImage[i].r = clampToU8(rgbI[2]);
	}

	input.close();

	ofstream output(outputFile, ios::binary);
	if (!output) {
		cerr << "Cannot open output file." << endl;
		return 1;
	}

	output.write(reinterpret_cast<char*>(&header), sizeof(TGAHeader));
	output.write(reinterpret_cast<char*>(outputImage.data()), pixelCount * sizeof(Pixel));
	output.close();

	cout << "Decoded successfully (k=" << k << ")." << endl;
	return 0;
}
