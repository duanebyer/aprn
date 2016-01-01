#include "include/integer.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstdlib>

using namespace aprn;

int main(int argc, char** argv) {
  int num_wrong = 0;
  std::cout << std::setbase(16);
  std::srand(std::time(0));
  for (int i = 0; i < 1000000; ++i) {
    
    long long int_a = std::rand();
    long long int_b = std::rand();
    long long int_sum = int_a + int_b;
    
    Integer integer_a = Integer(int_a);
    Integer integer_b = Integer(int_b);
    
    Integer integer_predicted_sum = integer_a + integer_b;
    Integer integer_sum = Integer(int_sum);
    
    /*if (integer_predicted_sum.m_digits.size() != integer_sum.m_digits.size()) {
      std::cout << "sizes are different\n";
      Integer new_integer_sum = Integer(int_sum);
      }*/
    
    if (integer_sum != integer_predicted_sum) {
      std::cout << "WRONG, should be " << int_sum << ": \n";
      std::cout << '\t' << integer_a << " + " << integer_b << " = " << integer_predicted_sum << '\n';
      ++num_wrong;
    }
  }
  std::cout << std::setbase(10);
  std::cout << "number of incorrect sums: " <<  num_wrong << '\n';
  std::cout << std::setbase(16);
  return 0;
}
