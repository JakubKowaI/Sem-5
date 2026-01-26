#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

static bool parse_probability(const std::string& s, double& p) {
	try {
		size_t idx = 0;
		p = std::stod(s, &idx);
		return idx == s.size();
	} catch (...) {
		return false;
	}
}

int main(int argc, char** argv) {
	if (argc != 4) {
		std::cerr << "Usage: szum p in out\n";
		return 2;
	}

	double p = 0.0;
	if (!parse_probability(argv[1], p) || p < 0.0 || p > 1.0) {
		std::cerr << "Invalid p (expected 0..1): " << argv[1] << "\n";
		return 2;
	}

	const std::string in_path = argv[2];
	const std::string out_path = argv[3];

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

	std::random_device rd;
	std::mt19937 rng(rd());
	std::bernoulli_distribution flip(p);

	char c;
	while (in.get(c)) {
		uint8_t b = static_cast<uint8_t>(static_cast<unsigned char>(c));
		for (int bit = 0; bit < 8; ++bit) {
			if (flip(rng)) {
				b ^= static_cast<uint8_t>(1U << bit);
			}
		}
		out.put(static_cast<char>(b));
	}

	return 0;
}
