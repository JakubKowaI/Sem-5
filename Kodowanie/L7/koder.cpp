#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include "hamming_ext74.h"

int main(int argc, char** argv) {
	if (argc != 3) {
		std::cerr << "Usage: koder in out\n";
		return 2;
	}

	const std::string in_path = argv[1];
	const std::string out_path = argv[2];

	std::ifstream in(in_path, std::ios::binary);
	if (!in) {
		std::cerr << "Cannot open input file: " << in_path << "\n";
		return 1;
	}
	std::ofstream out(out_path, std::ios::binary);
	if (!out) {
		std::cerr << "Cannot open output file: " << out_path << "\n";
		return 1;
	}

	char c;
	while (in.get(c)) {
		uint8_t b = static_cast<uint8_t>(static_cast<unsigned char>(c));
		uint8_t high = static_cast<uint8_t>((b >> 4) & 0x0F);
		uint8_t low = static_cast<uint8_t>(b & 0x0F);

		uint8_t enc_high = hamming_ext74::encode_nibble(high);
		uint8_t enc_low = hamming_ext74::encode_nibble(low);

		out.put(static_cast<char>(enc_high));
		out.put(static_cast<char>(enc_low));
	}

	return 0;
}
