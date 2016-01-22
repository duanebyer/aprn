#include "../include/rational.h"

#include <climits>
#include <cmath>

#include "../include/math_integer.h"

using namespace aprn;

Rational::Rational(Integer val) : m_numerator(val), m_denominator(1) {}

Rational::Rational(float val) : Rational((long double) val) {}

Rational::Rational(double val) : Rational((long double) val) {}

Rational::Rational(long double val) {
  const long double MAX_VALUE = std::ldexp(1.0L, std::numeric_limits<int>::max());
  const long double MIN_VALUE = std::ldexp(1.0L, std::numeric_limits<int>::min());
  
  unsigned long long power = 0;
  while (std::abs(val) >= MAX_VALUE) {
    val /= MAX_VALUE;
    power += std::numeric_limits<int>::max();
  }
  while (std::abs(val) <= MIN_VALUE) {
    val *= MIN_VALUE;
    power -= std::numeric_limits<int>::min();
  }
  int small_power;
  long double fractional = std::abs(std::frexp(val, small_power));
  m_numerator = (unsigned long long) (fractional * (1ULL << LDBL_MANT_DIG));
  power += small_power;
  power -= LDBL_MANT_DIG;
  
  if (power >= 0) {
    m_numerator <<= power;
    m_denominator = 1;
  }
  else {
    m_denominator = 1;
    m_denominator <<= -power;
  }
  
  if (val < 0) {
    m_numerator.negate();
  }
  
  makeValid();
}
