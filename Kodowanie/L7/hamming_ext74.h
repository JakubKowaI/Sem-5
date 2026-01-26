#pragma once

#include <array>
#include <cstdint>

namespace hamming_ext74 {

constexpr uint8_t generator_poly = 0b1011; // 1 + x + x^3

inline uint8_t popcount8(uint8_t v) {
	// Portable popcount for 8-bit
	v = static_cast<uint8_t>(v - ((v >> 1) & 0x55));
	v = static_cast<uint8_t>((v & 0x33) + ((v >> 2) & 0x33));
	return static_cast<uint8_t>(((v + (v >> 4)) & 0x0F));
}

inline uint8_t remainder_mod_g_7bit(uint8_t poly7) {
	// poly7 uses bits 0..6 as coefficients of x^0..x^6
	uint8_t tmp = static_cast<uint8_t>(poly7 & 0x7F);
	for (int i = 6; i >= 3; --i) {
		if ((tmp >> i) & 1U) {
			tmp ^= static_cast<uint8_t>(generator_poly << (i - 3));
		}
	}
	return static_cast<uint8_t>(tmp & 0x07);
}

inline uint8_t encode_nibble(uint8_t nibble) {
	// nibble bits are coefficients of x^0..x^3 (LSB is x^0)
	uint8_t msg = static_cast<uint8_t>(nibble & 0x0F);
	uint8_t shifted = static_cast<uint8_t>(msg << 3);
	uint8_t rem = remainder_mod_g_7bit(shifted);
	uint8_t code7 = static_cast<uint8_t>((shifted | rem) & 0x7F);
	uint8_t overall_parity = static_cast<uint8_t>(popcount8(code7) & 1U); // set to make total parity even
	return static_cast<uint8_t>(code7 | (overall_parity << 7));
}

inline std::array<int8_t, 8> syndrome_to_bitpos() {
	// Maps 3-bit syndrome (0..7) to bit position in code7 (0..6). -1 means unknown.
	std::array<int8_t, 8> map{};
	map.fill(-1);
	for (int bit = 0; bit < 7; ++bit) {
		uint8_t syn = remainder_mod_g_7bit(static_cast<uint8_t>(1U << bit));
		map[syn] = static_cast<int8_t>(bit);
	}
	return map;
}

struct DecodeResult {
	uint8_t nibble = 0;
	bool corrected_single_error = false;
	bool detected_double_error = false;
};

inline DecodeResult decode_codeword(uint8_t code8) {
	static const std::array<int8_t, 8> syn_map = syndrome_to_bitpos();

	DecodeResult r;
	uint8_t code7 = static_cast<uint8_t>(code8 & 0x7F);
	uint8_t syndrome = remainder_mod_g_7bit(code7);
	uint8_t overall_parity_check = static_cast<uint8_t>(popcount8(code8) & 1U); // 0 means OK (even)

	if (syndrome == 0 && overall_parity_check == 0) {
		// no error
	} else if (syndrome == 0 && overall_parity_check == 1) {
		// error only in overall parity bit (bit 7)
	} else if (syndrome != 0 && overall_parity_check == 1) {
		// single-bit error in bits 0..6
		int8_t pos = syn_map[syndrome];
		if (pos >= 0 && pos < 7) {
			code7 ^= static_cast<uint8_t>(1U << pos);
			r.corrected_single_error = true;
		}
	} else {
		// syndrome != 0 && overall_parity_check == 0 => detected double (or other even-count) error
		r.detected_double_error = true;
	}

	r.nibble = static_cast<uint8_t>((code7 >> 3) & 0x0F);
	return r;
}

} // namespace hamming_ext74
