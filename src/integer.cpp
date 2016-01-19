#include "../include/integer.h"

#include <algorithm>
#include <climits>
#include <cstdint>
#include <iomanip>
#include <istream>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "../include/math_integer.h"

using namespace aprn;

bool isBigEndian();

Integer::Digit const Integer::MAX_DIGIT = std::numeric_limits<Digit>::max();

Integer::Integer() {
  m_isNegative = false;
}

Integer::Integer(signed char val) : Integer((signed long long) val) {}
Integer::Integer(unsigned char val) : Integer((unsigned long long) val) {}
Integer::Integer(signed short val) : Integer((signed long long) val) {}
Integer::Integer(unsigned short val) : Integer((unsigned long long) val) {}
Integer::Integer(signed int val) : Integer((signed long long) val) {}
Integer::Integer(unsigned int val) : Integer((unsigned long long) val) {}
Integer::Integer(signed long val) : Integer((signed long long) val) {}
Integer::Integer(unsigned long val) : Integer((unsigned long long) val) {}

Integer::Integer(signed long long val) : Integer((unsigned long long) (std::abs(val))) {
  m_isNegative = val < 0;
}

Integer::Integer(unsigned long long val) {
  std::size_t bytes = sizeof(val);
  std::size_t bytesPerDigit = sizeof(Digit);
  Digit* valPtr = reinterpret_cast<Digit*>(&val);
  
  m_digits.resize(bytes / bytesPerDigit);
  m_isNegative = false;
  
  for (SizeType i = 0; i < m_digits.size(); ++i) {
    m_digits[i] = valPtr[!isBigEndian() ? i : m_digits.size() - i - 1];
  }
  makeValid();
}

Integer::operator signed char() const { return (signed char) operator signed long long(); }
Integer::operator unsigned char() const { return (unsigned char) operator unsigned long long(); }
Integer::operator signed short() const { return (signed short) operator signed long long(); }
Integer::operator unsigned short() const { return (unsigned short) operator unsigned long long(); }
Integer::operator signed int() const { return (signed int) operator signed long long(); }
Integer::operator unsigned int() const { return (unsigned int) operator unsigned long long(); }
Integer::operator signed long() const { return (signed long) operator signed long long(); }
Integer::operator unsigned long() const { return (unsigned long) operator unsigned long long(); }

Integer::operator signed long long() const {
  std::uintmax_t val = operator std::uintmax_t();
  val %= INTMAX_MAX;
  val = m_isNegative ? -val : val;
  return val;
}

Integer::operator unsigned long long() const {
  std::uintmax_t result = 0;
  std::size_t numDigits = sizeof(std::uintmax_t) / sizeof(Digit) +
    ((sizeof(std::uintmax_t) % sizeof(Digit)) != 0);
  numDigits = std::min(numDigits, m_digits.size());
  for (std::size_t i = 0; i < numDigits; ++i) {
    std::uintmax_t digit = (std::uintmax_t) m_digits[i];
    digit <<= CHAR_BIT * sizeof(Digit) * i;
    result |= digit;
  }
  return result;
}

// Takes an integer in invalid form and makes it valid.
void Integer::makeValid() {
  while (m_digits.size() != 0 && m_digits.back() == 0) {
    m_digits.pop_back();
  }
  m_isNegative = m_isNegative && (m_digits.size() != 0);
}

bool isBigEndian() {
  union {
    std::uint32_t i;
    char c[4];
  } x = { 0x01020304 };
  return x.c[0] == 1;
}

bool aprn::operator==(Integer const& lhs, Integer const& rhs) {
  if (lhs.m_isNegative != rhs.m_isNegative) {
    return false;
  }
  else {
    return Integer::compareMagnitude(lhs, rhs) == 0;
  }
}

bool aprn::operator<(Integer const& lhs, Integer const& rhs) {
  if (lhs.m_isNegative != rhs.m_isNegative) {
    return lhs.m_isNegative;
  }
  else {
    return Integer::compareMagnitude(lhs, rhs) == -signum(lhs);
  }
}

int Integer::compareMagnitude(Integer const& lhs, Integer const& rhs) {
  if (lhs.m_digits.size() > rhs.m_digits.size()) {
    return 1;
  }
  else if (lhs.m_digits.size() < rhs.m_digits.size()) {
    return -1;
  }
  else if (lhs.m_digits.size() == 0) {
    return 0;
  }
  else {
    Integer::SizeType i = lhs.m_digits.size();
    do {
      --i;
      Integer::Digit lhsDigit = lhs.m_digits[i];
      Integer::Digit rhsDigit = rhs.m_digits[i];
      if (lhsDigit > rhsDigit) {
        return 1;
      }
      else if (lhsDigit < rhsDigit) {
        return -1;
      }
    } while (i != 0);
    return 0;
  }
}

Integer Integer::operator-() const {
  Integer result(*this);
  return result.negate();
}

Integer const& Integer::operator+() const {
  return *this;
}

Integer& Integer::operator+() {
  return *this;
}

Integer& Integer::negate() {
  m_isNegative = !m_isNegative && (m_digits.size() != 0);
  return *this;
}

Integer& Integer::operator++() {
  switch (m_isNegative) {
  case true:
    negate();
    operator--();
    negate();
    break;
  case false:
    bool hasCarry = true;
    for (SizeType i = 0; i < m_digits.size(); ++i) {
      if (hasCarry) {
        hasCarry = (++m_digits[i] == 0);
      }
      else {
        break;
      }
    }
    if (hasCarry) {
      m_digits.push_back(1);
    }
    break;
  }
  return *this;
}

Integer& Integer::operator--() {
  switch (m_isNegative) {
  case true:
    negate();
    operator++();
    negate();
    break;
  case false:
    bool hasBorrow = true;
    for (SizeType i = 0; i < m_digits.size(); ++i) {
      if (hasBorrow) {
        hasBorrow = (--m_digits[i] == MAX_DIGIT);
      }
      else {
        break;
      }
    }
    if (hasBorrow) {
      m_isNegative = true;
      m_digits.push_back(1);
    }
    break;
  }
  return *this;
}

Integer& Integer::operator+=(Integer const& rhs) {
  if (m_isNegative == rhs.m_isNegative) {
    return addMagnitude(rhs);
  }
  else {
    return subtractMagnitude(rhs);
  }
}

Integer& Integer::addMagnitude(Integer const& rhs) {
  Integer::SizeType numDigits = std::max(m_digits.size(), rhs.m_digits.size());
  bool hasCarry = false;
  m_digits.resize(numDigits, 0);
  for (Integer::SizeType i = 0; i < numDigits; ++i) {
    Integer::Digit lhsDigit = m_digits[i];
    Integer::Digit rhsDigit = i < rhs.m_digits.size() ? rhs.m_digits[i] : 0;
    Integer::Digit maxDigit = std::max(lhsDigit, rhsDigit);
    
    m_digits[i] += rhsDigit + hasCarry;
    hasCarry = (!hasCarry && m_digits[i] < maxDigit) ||
      (hasCarry && m_digits[i] <= maxDigit);
  }
  if (hasCarry) {
    m_digits.push_back(1);
  }
  return *this;
}

Integer& Integer::operator-=(Integer const& rhs) {
  if (m_isNegative == rhs.m_isNegative) {
    return subtractMagnitude(rhs);
  }
  else {
    return addMagnitude(rhs);
  }
}

Integer& Integer::subtractMagnitude(Integer const& rhs) {
  int compMag = compareMagnitude(*this, rhs);
  Integer::SizeType numDigits = std::max(m_digits.size(), rhs.m_digits.size());
  bool hasBorrow = false;
  m_digits.resize(numDigits, 0);
  if (compMag == 0) {
    m_digits.clear();
    m_isNegative = false;
    return *this;
  }
  for (Integer::SizeType i = 0; i < numDigits; ++i) {
    Integer::Digit lhsDigit = m_digits[i];
    Integer::Digit rhsDigit = i < rhs.m_digits.size() ? rhs.m_digits[i] : 0;
    
    m_digits[i] = (lhsDigit - rhsDigit) * compMag - hasBorrow;
    Integer::Digit compDigit = (compMag > 0) * lhsDigit + (compMag < 0) * rhsDigit;
    hasBorrow = (!hasBorrow && m_digits[i] > compDigit) ||
      (hasBorrow && m_digits[i] >= compDigit);
  }
  if (compMag < 0) {
    m_isNegative = !m_isNegative;
  }
  makeValid();
  return *this;
}

Integer& Integer::operator*=(Integer const& rhs) {
  return setToProduct(Integer(*this), rhs);
}

Integer aprn::operator*(Integer const& lhs, Integer const& rhs) {
  Integer result = Integer();
  result.setToProduct(lhs, rhs);
  return result;
}


Integer& Integer::setToProduct(Integer const& lhs, Integer const& rhs) {
  Integer::Digit carry = 0;
  
  m_digits.clear();
  m_isNegative = false;
  
  for (Integer::SizeType i = 0; i < rhs.m_digits.size(); ++i) {
    Integer::Digit rhsDigit = rhs.m_digits[i];
    Integer nextSum;
    nextSum.m_digits.resize(i, 0);
    carry = 0;
    for (Integer::SizeType j = 0; j < lhs.m_digits.size(); ++j) {
      Integer::Digit lhsDigit = lhs.m_digits[j];
      Integer::DoubleDigit digitProduct = lhsDigit * rhsDigit + carry;
      carry = reinterpret_cast<Integer::Digit*>(&digitProduct)[isBigEndian() ? 0 : 1];
      nextSum.m_digits.push_back(reinterpret_cast<Integer::Digit*>(&digitProduct)[isBigEndian() ? 1 : 0]);
    }
    nextSum.m_digits.push_back(carry);
    nextSum.makeValid();
    operator+=(nextSum);
  }
  
  m_isNegative = lhs.m_isNegative ^ rhs.m_isNegative;
  
  makeValid();
  
  return *this;
}

Integer& Integer::operator/=(Integer const& rhs) {
  Integer rem = Integer();
  Integer::quotRem(Integer(*this), rhs, *this, rem);
  return *this;
}

Integer aprn::operator/(Integer const& lhs, Integer const& rhs) {
  Integer quot = Integer();
  Integer rem = Integer();
  Integer::quotRem(lhs, rhs, quot, rem);
  return quot;
}

Integer& Integer::operator%=(Integer const& rhs) {
  Integer quot = Integer();
  Integer::quotRem(Integer(*this), rhs, quot, *this);
  return *this;
}

Integer aprn::operator%(Integer const& lhs, Integer const& rhs) {
  Integer quot = Integer();
  Integer rem = Integer();
  Integer::quotRem(lhs, rhs, quot, rem);
  return rem;
}

bool aprn::Integer::quotRem(Integer const& lhs, Integer const& rhs, Integer& quot_out, Integer& rem_out) {
  if (lhs < rhs) {
    quot_out = Integer();
    rem_out = Integer(lhs);
  }
  
  int lhsSign = signum(lhs);
  int rhsSign = signum(rhs);
  int quotSign = lhsSign * rhsSign;
  Integer currentDividend = Integer();
  
  if (signum(rhs) == 0) {
    return false;
  }
  
  quot_out.m_digits.resize(lhs.m_digits.size(), 0);
  
  Integer::SizeType i = lhs.m_digits.size();
  do {
    --i;
    currentDividend.m_digits.insert(currentDividend.m_digits.begin(), lhs.m_digits[i]);
    Integer::Digit currentQuotient;
    Integer::SizeType currentDividendNumDigits = currentDividend.m_digits.size();
    Integer::SizeType rhsNumDigits = rhs.m_digits.size();
    if (currentDividendNumDigits < rhsNumDigits) {
      currentQuotient = 0;
    }
    else {
      Integer::DoubleDigit rhsEnd = rhs.m_digits.back();
      Integer::DoubleDigit dividendEnd;
      if (currentDividendNumDigits == rhsNumDigits) {
        dividendEnd = currentDividend.m_digits.back();
      }
      else {
        dividendEnd = currentDividend.m_digits.back() * ((Integer::DoubleDigit) Integer::MAX_DIGIT + 1);
        dividendEnd += currentDividend.m_digits[currentDividendNumDigits - 2];
      }
      currentQuotient = dividendEnd / (rhsEnd + 1);
      Integer quotientProduct = rhs * currentQuotient;
      quotientProduct.m_isNegative = false;
      
      for (Integer::DoubleDigit quotient = currentQuotient + 1; quotient <= Integer::MAX_DIGIT; ++quotient) {
        Integer nextQuotientProduct = rhs * quotient;
        nextQuotientProduct.m_isNegative = false;
        if (nextQuotientProduct > currentDividend) {
          break;
        }
        else {
          quotientProduct = nextQuotientProduct;
          currentQuotient = quotient;
        }
      }
      
      currentDividend -= quotientProduct;
    }
    
    quot_out.m_digits[i] = currentQuotient;
  } while (i != 0);
  
  rem_out = currentDividend;
  
  quot_out.makeValid();
  rem_out.makeValid();
  
  if (quotSign == -1) {
    quot_out.negate();
  }
  if (lhsSign == -1) {
    rem_out.negate();
  }
  return true;
}

Integer Integer::operator~() const {
  Integer result;
  result.m_digits.resize(m_digits.size());
  for (SizeType i = 0; i < m_digits.size(); ++i) {
    result.m_digits[i] = ~m_digits[i];
  }
  result.m_isNegative = !m_isNegative;
  return result;
}

Integer& Integer::operator&=(Integer const& rhs) {
  if (rhs.m_digits.size() > m_digits.size()) {
    m_digits.resize(rhs.m_digits.size(), 0);
  }
  for (SizeType i = 0; i < m_digits.size(); ++i) {
    m_digits[i] &= rhs.m_digits[i];
  }
  m_isNegative = rhs.m_isNegative && m_isNegative;
  makeValid();
  return *this;
}

Integer& Integer::operator|=(Integer const& rhs) {
  if (rhs.m_digits.size() > m_digits.size()) {
    m_digits.resize(rhs.m_digits.size(), 0);
  }
  for (SizeType i = 0; i < m_digits.size(); ++i) {
    m_digits[i] |= rhs.m_digits[i];
  }
  m_isNegative = rhs.m_isNegative || m_isNegative;
  return *this;
}

Integer& Integer::operator^=(Integer const& rhs) {
  if (rhs.m_digits.size() > m_digits.size()) {
    m_digits.resize(rhs.m_digits.size());
  }
  for (SizeType i = 0; i < m_digits.size(); ++i) {
    m_digits[i] ^= rhs.m_digits[i];
  }
  m_isNegative = rhs.m_isNegative ^ m_isNegative;
  makeValid();
  return *this;
}

Integer& Integer::operator>>=(ShiftType rhs) {
  Integer rem = Integer();
  return shiftRight(rhs, rem);
}

Integer& Integer::shiftRight(ShiftType rhs, Integer& rem_out) {
  ShiftType numDigits = rhs / (CHAR_BIT * sizeof(Digit));
  ShiftType numBits = rhs % (CHAR_BIT * sizeof(Digit));
  rem_out.m_digits.resize(numDigits + 1, 0);
  rem_out.m_isNegative = false;
  for (SizeType i = 0; i < m_digits.size(); ++i) {
    Digit digit = m_digits[i];
    i >= numDigits ? m_digits[i - numDigits] : rem_out.m_digits[i] |=
      (digit >> numBits);
    i > numDigits ? m_digits[i - numDigits - 1] : rem_out.m_digits[i - 1] |=
      (digit << (CHAR_BIT * sizeof(Digit) - numBits));
  }
  m_digits.resize(m_digits.size() - numDigits);
  if (m_digits.back() == 0) {
    m_digits.pop_back();
    if (m_digits.size() == 0) {
      m_isNegative = false;
    }
  }
  return *this;
}

Integer& Integer::operator<<=(ShiftType rhs) {
  return shiftLeft(rhs);
}

Integer& Integer::shiftLeft(ShiftType rhs) {
  ShiftType numDigits = rhs / (CHAR_BIT * sizeof(Digit));
  ShiftType numBits = rhs % (CHAR_BIT * sizeof(Digit));
  m_digits.resize(m_digits.size() + numDigits + 1, 0);
  
  SizeType i = m_digits.size();
  do {
    --i;
    Digit digit = m_digits[i];
    m_digits[i + numDigits] |= (digit << numBits);
    m_digits[i + numDigits + 1] |= (digit >> (CHAR_BIT * sizeof(Digit) - numBits));
  } while (i != 0);
  if (m_digits.back() == 0) {
    m_digits.pop_back();
  }
  return *this;
}

std::ostream& aprn::operator<<(std::ostream& os, Integer const& obj) {
  int base = 10;
  int sign = signum(obj);
  std::string digits = os.flags() & std::ios::uppercase ?
    "0123456789ABCDEF" : "0123456789abcdef";
  
  switch (os.flags() & std::ios::basefield) {
  case std::ios::dec:
    base = 10;
    break;
  case std::ios::oct:
    base = 8;
    break;
  case std::ios::hex:
    base = 16;
    break;
  }
  
  if ((os.flags() & std::ios::showbase) && sign != 0) {
    if (base == 8) {
      os << '0';
    }
    else if (base == 16) {
      os << "0x";
    }
  }
  
  if (sign == 0) {
    os << '0';
  }
  else if (CHAR_BIT % 4 == 0 && base == 16) {
    os << (sign < 0 ? "-" : (os.flags() & std::ios::showpos ? "+" : ""));
    std::ios::fmtflags oldFlags = os.flags();
    os.unsetf(std::ios::showbase);
    os.unsetf(std::ios::showpos);
    Integer::SizeType i = obj.m_digits.size();
    do {
      --i;
      std::ostringstream nextDigitStream;
      nextDigitStream.flags(os.flags());
      nextDigitStream << std::setw(sizeof(Integer::Digit) * 2);
      nextDigitStream << std::setfill('0');
      nextDigitStream << +obj.m_digits[i];
      os << nextDigitStream.str();
    } while (i != 0);
    os.flags(oldFlags);
  }
  else {
    std::string output = "";
    std::string signStr = sign < 0 ? "-" : (os.flags() & std::ios::showpos ? "+" : "");
    Integer value = sign < 0 ? -obj : obj;
    while (true) {
      if (value < Integer(base) && !value.m_digits.empty()) {
        output.insert(output.begin(), digits[value.m_digits.front()]);
        break;
      }
      else {
        Integer remainder;
        if (base % 2 == 0) {
          Integer::ShiftType power = base == 8 ? 3 : 4;
          value.shiftRight(power, remainder);
        }
        else {
          Integer quotient;
          Integer::quotRem(value, base, quotient, remainder);
          value = quotient;
        }
        output.insert(output.begin(), digits[remainder.m_digits.empty() ? 0 : remainder.m_digits.front()]);
      }
    }
    os << signStr << output;
  }
  
  return os;
}

std::istream& aprn::operator>>(std::istream& is, Integer& obj) {
  if (/* no valid integer */true) {
    is.setstate(std::ios::failbit);
  }
  return is;
}

