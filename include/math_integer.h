#ifndef __APRN_MATH_INTEGER_H_
#define __APRN_MATH_INTEGER_H_

#include "integer.h"

namespace aprn {
  
  int signum(Integer const& val) {
    return (val.m_digits.size() != 0) * (1 - 2 * val.m_isNegative);
  }
  int signum(unsigned char val) {
    return val != 0;
  }
  int signum(signed char val) {
    return (val > 0) - (val < 0);
  }
  int signum(unsigned short val) {
    return val != 0;
  }
  int signum(signed short val) {
    return (val > 0) - (val < 0);
  }
  int signum(unsigned int val) {
    return val != 0;
  }
  int signum(signed int val) {
    return (val > 0) - (val < 0);
  }
  int signum(unsigned long val) {
    return val != 0;
  }
  int signum(signed long val) {
    return (val > 0) - (val < 0);
  }
  int signum(unsigned long long val) {
    return val != 0;
  }
  int signum(signed long long val) {
    return (val > 0) - (val < 0);
  }
  
  Integer abs(Integer val) {
    val.m_isNegative = 0;
    return val;
  }
  unsigned char abs(unsigned char val) {
    return val;
  }
  signed char abs(signed char val) {
    return val >= 0 ? val : -val;
  }
  unsigned short abs(unsigned short val) {
    return val;
  }
  signed short abs(signed short val) {
    return val >= 0 ? val : -val;
  }
  unsigned int abs(unsigned int val) {
    return val;
  }
  signed int abs(signed int val) {
    return val >= 0;
  }
  unsigned long abs(unsigned long val) {
    return val;
  }
  signed long abs(signed long val) {
    return val >= 0 ? val : -val;
  }
  unsigned long long abs(unsigned long long val) {
    return val;
  }
  signed long long abs(signed long long val) {
    return val >= 0 ? val : -val;
  }
  
  template<typename T> struct div_result {
    T quot;
    T rem;
    bool success;
  };
  div_result<Integer> div(Integer const& lhs, Integer const& rhs) {
    div_result<Integer> result;
    result.success = Integer::quotRem(lhs, rhs, result.quot, result.rem);
    return result;
  }
  div_result<unsigned char> div(unsigned char lhs, unsigned char rhs) {
    if (rhs == 0) {
      return { 0, 0, false };
    }
    return { (unsigned char) (lhs / rhs), (unsigned char) (lhs % rhs), true };
  }
  div_result<signed char> div(signed char lhs, signed char rhs) {
    if (rhs == 0) {
      return { 0, 0, false };
    }
    std::div_t result = std::div(lhs, rhs);
    return { (signed char) result.quot, (signed char) result.rem, true };
  }
  div_result<unsigned short> div(unsigned short lhs, unsigned short rhs) {
    if (rhs == 0) {
      return { 0, 0, false };
    }
    return { (unsigned short) (lhs / rhs), (unsigned short) (lhs % rhs), true };
  }
  div_result<signed short> div(signed short lhs, signed short rhs) {
    if (rhs == 0) {
      return { 0, 0, false };
    }
    std::div_t result = std::div(lhs, rhs);
    return { (signed short) result.quot, (signed short) result.rem, true };
  }
  div_result<unsigned int> div(unsigned int lhs, unsigned int rhs) {
    if (rhs == 0) {
      return { 0, 0, false };
    }
    return { (unsigned int) (lhs / rhs), (unsigned int) (lhs % rhs), true };
  }
  div_result<signed int> div(signed int lhs, signed int rhs) {
    if (rhs == 0) {
      return { 0, 0, false };
    }
    std::div_t result = std::div(lhs, rhs);
    return { (signed int) result.quot, (signed int) result.rem, true };
  }
  div_result<unsigned long> div(unsigned long lhs, unsigned long rhs) {
    if (rhs == 0) {
      return { 0, 0, false };
    }
    return { (unsigned long) (lhs / rhs), (unsigned long) (lhs % rhs), true };
  }
  div_result<signed long> div(signed long lhs, signed long rhs) {
    if (rhs == 0) {
      return { 0, 0, false };
    }
    std::ldiv_t result = std::div(lhs, rhs);
    return { (signed long) result.quot, (signed long) result.rem, true };
  }
  div_result<unsigned long long> div(unsigned long long lhs, unsigned long long rhs) {
    if (rhs == 0) {
      return { 0, 0, false };
    }
    return { (unsigned long long) (lhs / rhs), (unsigned long long) (lhs % rhs), true };
  }
  div_result<signed long long> div(signed long long lhs, signed long long rhs) {
    if (rhs == 0) {
      return { 0, 0, false };
    }
    std::lldiv_t result = std::div(lhs, rhs);
    return { (signed long long) result.quot, (signed long long) result.rem, true };
  }
  
  div_result<Integer> div2(Integer const& lhs, Integer::ShiftType power) {
    div_result<Integer> result = { lhs, Integer(), true };
    result.quot.shiftRight(power, result.rem);
    return result;
  }
  div_result<unsigned char> div2(unsigned char lhs, unsigned char power) {
    return { (unsigned char) (lhs >> power), (unsigned char) (lhs % (1 << power)), true };
  }
  div_result<signed char> div2(signed char lhs, signed char power) {
    if (power < 0) {
      return { 0, 0, false };
    }
    std::div_t result = std::div(lhs, 1 << power);
    return { (signed char) result.quot, (signed char) result.rem, true };
  }
  div_result<unsigned short> div2(unsigned short lhs, unsigned short power) {
    return { (unsigned short) (lhs >> power), (unsigned short) (lhs % (1 << power)), true };
  }
  div_result<signed short> div2(signed short lhs, signed short power) {
    if (power < 0) {
      return { 0, 0, false };
    }
    std::div_t result = std::div(lhs, 1 << power);
    return { (signed short) result.quot, (signed short) result.rem, true };
  }
  div_result<unsigned int> div2(unsigned int lhs, unsigned int power) {
    return { (unsigned int) (lhs >> power), (unsigned int) (lhs % (1 << power)), true };
  }
  div_result<signed int> div2(signed int lhs, signed int power) {
    if (power < 0) {
      return { 0, 0, false };
    }
    std::div_t result = std::div(lhs, 1 << power);
    return { (signed int) result.quot, (signed int) result.rem, true };
  }
  div_result<unsigned long> div2(unsigned long lhs, unsigned long power) {
    return { (unsigned long) (lhs >> power), (unsigned long) (lhs % (1 << power)), true };
  }
  div_result<signed long> div2(signed long lhs, signed long power) {
    if (power < 0) {
      return { 0, 0, false };
    }
    std::ldiv_t result = std::ldiv(lhs, 1 << power);
    return { (signed long) result.quot, (signed long) result.rem, true };
  }
  div_result<unsigned long long> div2(unsigned long long lhs, unsigned long long power) {
    return { (unsigned long long) (lhs >> power), (unsigned long long) (lhs % (1 << power)), true };
  }
  div_result<signed long long> div2(signed long long lhs, signed long long power) {
    if (power < 0) {
      return { 0, 0, false };
    }
    std::lldiv_t result = std::lldiv(lhs, 1 << power);
    return { (signed long long) result.quot, (signed long long) result.rem, true };
  }
  
}

#endif
