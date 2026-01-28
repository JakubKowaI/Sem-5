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
		auto in1 = read_all(argv[1]);
		auto in2 = read_all(argv[2]);

		const size_t max_bytes = (in1.size() > in2.size()) ? in1.size() : in2.size();
		std::uint64_t errors = 0;

		for (size_t i = 0; i < max_bytes; ++i) {
			bool hasblockin1 = i < in1.size();
			bool hasblockin2 = i < in2.size();

			uint8_t in1block = hasblockin1 ? in1[i] : 0;
			uint8_t in2block = hasblockin2 ? in2[i] : 0;

			uint8_t in1low = in1block & 0x0f;
			uint8_t in1high = in1block >>4;		

			uint8_t in2low = in2block & 0x0f;
			uint8_t in2high = in2block >>4;		

			// uint8_t a_hi = static_cast<uint8_t>((ba >> 4) & 0x0F);
			// uint8_t a_lo = static_cast<uint8_t>(ba & 0x0F);
			// uint8_t b_hi = static_cast<uint8_t>((bb >> 4) & 0x0F);
			// uint8_t b_lo = static_cast<uint8_t>(bb & 0x0F);

			if (!hasblockin1 || !hasblockin2 || in1high != in2high) {
				++errors;
			}
			if (!hasblockin1 || !hasblockin2 || in1low != in2low) {
				++errors;
			}
		}

		std::cout << errors << "\n";
		return 0;
	} catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return 1;
	}
}
