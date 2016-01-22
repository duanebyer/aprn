#include "../include/math_integer.h"

#include <climits>

using namespace aprn;

int aprn::signum(Integer const& val) {
  return (val.m_digits.size() != 0) * (1 - 2 * val.m_isNegative);
}

Integer aprn::abs(Integer val) {
  val.m_isNegative = 0;
  return val;
}

bool aprn::even(Integer const& val) {
  return val.m_digits.empty() || (val.m_digits.front() % 2 == 0);
}
  
div_result aprn::div(Integer const& lhs, Integer const& rhs) {
  div_result result;
  result.success = Integer::quotRem(lhs, rhs, result.quot, result.rem);
  return result;
}

div_result aprn::div2(Integer const& lhs, Integer::ShiftType power) {
  div_result result = { lhs, Integer(), true };
  result.quot.shiftRight(power, result.rem);
  return result;
}

Integer aprn::mod2(Integer const& lhs, Integer::ShiftType power) {
  // This function works by chopping off the last number of bits from the Integer and
  // putting them into a new Integer which is then returns.
  
  // The number of digits to chop of is given by numDigits, and then some bits  of the next
  // digit after that are also chopped off (numBits).
  Integer::ShiftType numDigits = power / (CHAR_BIT * sizeof(Integer::Digit));
  Integer::ShiftType numBits = power % (CHAR_BIT * sizeof(Integer::Digit));
  Integer result;
  if (lhs.m_digits.size() < numDigits) {
    return lhs;
  }
  // The digits are chopped off and packaged into the result.
  result.m_digits.insert(result.m_digits.begin(), lhs.m_digits.begin(), lhs.m_digits.begin() + numDigits);
  // The remaining bits are chopped off and packaged.
  if (lhs.m_digits.size() > numDigits && numBits != 0) {
    result.m_digits.push_back(lhs.m_digits[numDigits] >> (CHAR_BIT * sizeof(Integer::Digit) - numBits));
  }
  // The sign has to the the same as the left hand side.
  result.m_isNegative = lhs.m_isNegative;
  return result;
}

Integer aprn::gcd(Integer a, Integer b) {
  // This is just a straightforward implementation of the binary Euclid's algorithm.
  Integer::ShiftType resultPower = 0;
  // Instead of recursion, a loop is used to prevent going over the max recursive depth.
  while (true) {
    int compare = Integer::compareMagnitude(a, b);
    if (compare == 0) {
      a <<= resultPower;
      return a;
    }
    if (signum(a) == 0) {
      b <<= resultPower;
      return b;
    }
    if (signum(b) == 0) {
      a <<= resultPower;
      return a;
    }
    
    if (even(a) && even(b)) {
      a >>= 1;
      b >>= 1;
      ++resultPower;
    }
    else if (even(a)) {
      a >>= 1;
    }
    else if (even(b)) {
      b >>= 1;
    }
    else if (compare > 0) {
      a -= b;
      a >>= 1;
    }
    else {
      b -= a;
      b >>= 1;
    }
  }
}
