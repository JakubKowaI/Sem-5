#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <iostream>
#include <bitset>

namespace hamming {

constexpr uint8_t wielomian = 0b11010000; // 1 + x + x^3
constexpr uint8_t wielomianH = 0b00010111; // x^4 + x^2 + x + 1

std::array<uint8_t,4> matrix{};
std::array<uint8_t,3> matrixH{};
std::array<uint8_t,7> matrixT{};

void gen_matrix(){
	for(int y=0;y<4;y++){
		uint8_t tmp = static_cast<uint8_t>(wielomian >> y);
		matrix[y] = 0u;
		for(int x=0;x<8;x++){
			bool is_one = tmp & 0b1;
			if(is_one) matrix[y] |= static_cast<uint8_t>(1u << x);
			tmp = tmp>>1;
		}
	}
	for(int y=0;y<4;y++){
		uint8_t tmp=matrix[y];
		tmp=tmp>>1;
		uint8_t count=0;
		for(int c=0;c<7;c++){
			if(tmp%2)count++;
			tmp=tmp>>1;
		}
		matrix[y]=matrix[y]>>1;
		matrix[y]=matrix[y]<<1;
		if(count%2==1){
			matrix[y]+=1;
		}
	}

	// for(auto i : matrix){
	// 	std::cout<<std::bitset<8>(i)<<std::endl;
	// }

	for(int i=0;i<3;i++){
		matrixH[i]=wielomianH<<i;
	}

	for(int j=6;j>=0;j--){
		uint8_t tmp=0;
		for(int i=0;i<4;i++){
			tmp=tmp<<1;
			tmp+=(matrix[i]>>j+1)%2;
		}
		matrixT[6-j]=tmp;
	}

	std::cout<<"matrixT: \n";
	for(auto i : matrixT){
		std::cout<<std::bitset<8>(i)<<std::endl;
	}
}

inline uint8_t encode(uint8_t info) {
	uint8_t msg = static_cast<uint8_t>(info & 0x0F);
	uint8_t code = 0;

	//std::cout<<"Wiadomość: " + std::bitset<8>(msg).to_string()+"Kodowanie"<<std::endl;
	for (int row = 0; row < 4; row++) {
		if ((msg >> row) % 2) {
			//std::cout<<std::bitset<8>(matrix[3-row])<<std::endl;
			code ^= matrix[3-row];
		}
	}

	return code;
}

struct DecodeResult {
	uint8_t nibble = 0;
	bool corrected_single_error = false;
	bool detected_double_error = false;
};

inline DecodeResult decode_codeword(uint8_t code8) {
	static std::array<int8_t, 256> code_to_nibble;
	static bool inited = false;
	if (!inited) {
		code_to_nibble.fill(-1);
		for (int m = 0; m < 16; ++m) {
			uint8_t c = encode(static_cast<uint8_t>(m));
			code_to_nibble[c] = static_cast<int8_t>(m);
		}
		inited = true;
	}

	DecodeResult r;
	int8_t exact = code_to_nibble[code8];
	if (exact >= 0) {
		r.nibble = static_cast<uint8_t>(exact);
		return r;
	}

	// try single-bit correction
	for (int pos = 0; pos < 8; ++pos) {
		uint8_t flipped = static_cast<uint8_t>(code8 ^ (1u << pos));
		int8_t v = code_to_nibble[flipped];
		if (v >= 0) {
			r.nibble = static_cast<uint8_t>(v);
			r.corrected_single_error = true;
			return r;
		}
	}

	r.detected_double_error = true;
	return r;
}

// inline DecodeResult decode_codeword(uint8_t code8){
// 	uint8_t syndrom=0;
// 	for(int i=0;i<3;i++){
// 		uint8_t tmp = matrixH[i] & code8;
// 		uint8_t count=0;
// 		syndrom = syndrom<<1;
// 		for(int j=0;j<7;j++){
// 			if((tmp>>j)%2)count++;
// 		}
// 		syndrom=syndrom>>1;
// 		syndrom=syndrom<<1;
// 		if(count%2){
// 			syndrom+=1;
// 		}
// 	}

// 	DecodeResult r;

// 	//Naprawić przekaz
// 	if(syndrom>0){
// 		r.corrected_single_error=1;
// 		code8 ^= ((uint8_t)1 << syndrom);
			
// 		uint8_t count=0;
// 		for(int i=0;i<8;i++){
// 			if((code8>>i)%2)count++;
// 		}
// 		if(count%2)r.detected_double_error=1;

// 		r.nibble=0;
// 		for(int i=0;i<7;i++){
// 			//uint8_t tmp =code8>>i;
// 			if((code8>>i)%2){
// 				r.nibble^=matrixT[i];
// 			}
// 		}

// 		//r.nibble=(code8>>1)^0b1011;
// 		return r;
// 	}
	

// 	uint8_t count=0;
// 	for(int i=0;i<8;i++){
// 		if((code8>>i)%2)count++;
// 	}
// 	if(count%2)r.corrected_single_error=1;
// 	//code8 ^= (uint8_t)1;
// 	r.nibble=0;
// 	std::cout<<"Dekodowanie\n";
// 	for(int i=0;i<7;i++){
		
// 		if((code8>>(i+1))%2){
// 			std::cout<<std::bitset<8>(matrixT[i]).to_string()<<std::endl;
// 			r.nibble^=matrixT[i];
// 		}
// 	}
// 	return r;
// }


}
