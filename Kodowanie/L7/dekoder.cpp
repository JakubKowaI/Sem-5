#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include "hamming_ext74.h"

int main(int argc, char** argv) {
	if (argc != 3) {
		std::cerr << "Usage: dekoder in out\n";
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

	std::uint64_t two_errors = 0;

	char c1;
	char c2;
	while (true) {
		if (!in.get(c1)) {
			break;
		}
		if (!in.get(c2)) {
			std::cerr << "Warning: input has odd number of bytes; last byte ignored.\n";
			break;
		}

		uint8_t b1 = static_cast<uint8_t>(static_cast<unsigned char>(c1));
		uint8_t b2 = static_cast<uint8_t>(static_cast<unsigned char>(c2));

		auto r1 = hamming_ext74::decode_codeword(b1);
		auto r2 = hamming_ext74::decode_codeword(b2);
		if (r1.detected_double_error) {
			++two_errors;
		}
		if (r2.detected_double_error) {
			++two_errors;
		}

		uint8_t out_byte = static_cast<uint8_t>(((r1.nibble & 0x0F) << 4) | (r2.nibble & 0x0F));
		out.put(static_cast<char>(out_byte));
	}

	// As per spec: print how many times we hit 2 errors.
	std::cout << two_errors << "\n";
	return 0;
}
