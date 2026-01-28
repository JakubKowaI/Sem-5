#include "hamming.h"

int main(){
    hamming::gen_matrix();
    // std::cout<<std::bitset<8>(hamming::encode(0b00000011))<<std::endl;
    std::cout<<hamming::encode(0b00000011)<<std::endl;
}