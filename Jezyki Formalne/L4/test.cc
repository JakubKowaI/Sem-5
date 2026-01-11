#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

std::vector<std::string> code;

  void print(std::string instruction) {
      code.push_back(instruction);
  }

void gen_const_to_reg(unsigned long value, char reg = 'a') {
      std::string r(1, reg);
      print("RST " + r);
      if (value == 0) return;
      
      // Znajdź najbardziej znaczący bit
      unsigned long temp = value;
      std::string binary = "";
      while(temp > 0){
          binary += (temp % 2 == 0 ? "0" : "1");
          temp /= 2;
      }
      std::reverse(binary.begin(), binary.end());

      for (size_t i = 0; i < binary.length(); i++) {
          if (i > 0) print("SHL " + r); 
          if (binary[i] == '1') print("INC " + r); // Dodaj 1
      }
  }

int main(){
    gen_const_to_reg(5,'b');
    for (const auto& ins : code) {
          std::cout << ins << std::endl;
      }

}