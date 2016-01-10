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
  int num_tests = 100000000;
  for (int i = 0; i < num_tests; ++i) {
    
    if (i % (num_tests / 100) == 0) {
      std::cout << std::setbase(10);
      std::cout << i / (num_tests / 100) << "%\n";
      std::cout << std::setbase(16);
    }
    
    long long int_a = std::rand() - RAND_MAX / 2;
    long long int_b = std::rand() - RAND_MAX / 2;
    while (int_b == 0) {
      int_b = std::rand();
    }
    long long int_result = int_a % int_b;
    
    Integer integer_a = Integer(int_a);
    Integer integer_b = Integer(int_b);
    
    Integer integer_predicted_result = integer_a % integer_b;
    Integer integer_result = Integer(int_result);
    
    if (integer_result == integer_predicted_result) {
      //std::cout << '\t';
    }
    else {
      std::cout << "Should be " << integer_result << ": ";
      std::cout << integer_a << " + " << integer_b << " = " << integer_predicted_result << '\n';
      
      ++num_wrong;
    }
    //std::cout << integer_a << " + " << integer_b << " = " << integer_predicted_result << '\n';
  }
  std::cout << std::setbase(10);
  std::cout << "number of incorrect sums: " <<  num_wrong << '\n';
  std::cout << std::setbase(16);
  return 0;
}
