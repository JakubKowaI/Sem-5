#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

static std::vector<uint8_t> read_all(const std::string& path) {
	std::ifstream in(path, std::ios::binary);
	if (!in) {
		throw std::runtime_error("Cannot open file: " + path);
	}
	std::vector<uint8_t> data;
	in.seekg(0, std::ios::end);
	std::streampos size = in.tellg();
	in.seekg(0, std::ios::beg);
	if (size > 0) {
		data.resize(static_cast<size_t>(size));
		in.read(reinterpret_cast<char*>(data.data()), size);
	}
	return data;
}

int main(int argc, char** argv) {
	if (argc != 3) {
		std::cerr << "Usage: sprawdz in1 in2\n";
		return 2;
	}

	try {
		auto a = read_all(argv[1]);
		auto b = read_all(argv[2]);

		const size_t max_bytes = (a.size() > b.size()) ? a.size() : b.size();
		std::uint64_t mismatched_nibbles = 0;

		for (size_t i = 0; i < max_bytes; ++i) {
			bool have_a = i < a.size();
			bool have_b = i < b.size();

			uint8_t ba = have_a ? a[i] : 0;
			uint8_t bb = have_b ? b[i] : 0;

			uint8_t a_hi = static_cast<uint8_t>((ba >> 4) & 0x0F);
			uint8_t a_lo = static_cast<uint8_t>(ba & 0x0F);
			uint8_t b_hi = static_cast<uint8_t>((bb >> 4) & 0x0F);
			uint8_t b_lo = static_cast<uint8_t>(bb & 0x0F);

			if (!have_a || !have_b || a_hi != b_hi) {
				++mismatched_nibbles;
			}
			if (!have_a || !have_b || a_lo != b_lo) {
				++mismatched_nibbles;
			}
		}

		std::cout << mismatched_nibbles << "\n";
		return 0;
	} catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return 1;
	}
}
