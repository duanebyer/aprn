#include "../include/integer.h"

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <istream>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

using namespace aprn;

void makeValid(Integer& val);
bool isBigEndian();

Integer& addMagnitude(Integer& lhs, Integer const& rhs);
Integer& subtractMagnitude(Integer& lhs, Integer const& rhs);
int compareMagnitude(Integer const& lhs, Integer const& rhs);

Integer& multiply(Integer const& lhs, Integer const& rhs, Integer& result_out);

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

Integer::Integer(signed long long val) {
  int bytes = sizeof(val);
  int bytesPerDigit = sizeof(Digit);
  bool isNegative = val < 0;
  Digit* valPtr = reinterpret_cast<Digit*>(&val);
  
  val = isNegative ? -val : +val;
  m_digits.resize(bytes / bytesPerDigit);
  m_isNegative = isNegative;
  
  for (SizeType i = 0; i < m_digits.size(); ++i) {
    m_digits[i] = valPtr[!isBigEndian() ? i : m_digits.size() - i - 1];
  }
  makeValid(*this);
}

Integer::Integer(unsigned long long val) {
  int bytes = sizeof(val);
  int bytesPerDigit = sizeof(Digit);
  Digit* valPtr = reinterpret_cast<Digit*>(&val);
  
  m_digits.resize(bytes / bytesPerDigit);
  m_isNegative = false;
  
  for (SizeType i = 0; i < m_digits.size(); ++i) {
    m_digits[i] = valPtr[!isBigEndian() ? i : m_digits.size() - i - 1];
  }
  makeValid(*this);
}

// Takes an integer in invalid form and makes it valid.
void makeValid(Integer& val) {
  while (val.m_digits.size() != 0 && val.m_digits.back() == 0) {
    val.m_digits.pop_back();
  }
  val.m_isNegative = val.m_isNegative && (val.m_digits.size() != 0);
}

bool isBigEndian() {
  union {
    std::uint32_t i;
    char c[4];
  } x = { 0x01020304 };
  return x.c[0] == 1;
}

int aprn::signum(Integer const& val) {
  return (val.m_digits.size() != 0) * (1 - 2 * val.m_isNegative);
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
    return addMagnitude(*this, rhs);
  }
  else {
    return subtractMagnitude(*this, rhs);
  }
}

Integer& addMagnitude(Integer& lhs, Integer const& rhs) {
  Integer::SizeType numDigits = std::max(lhs.m_digits.size(), rhs.m_digits.size());
  bool hasCarry = false;
  lhs.m_digits.resize(numDigits, 0);
  for (Integer::SizeType i = 0; i < numDigits; ++i) {
    Integer::Digit lhsDigit = lhs.m_digits[i];
    Integer::Digit rhsDigit = i < rhs.m_digits.size() ? rhs.m_digits[i] : 0;
    Integer::Digit maxDigit = std::max(lhsDigit, rhsDigit);
    
    lhs.m_digits[i] += rhsDigit + hasCarry;
    hasCarry = (!hasCarry && lhs.m_digits[i] < maxDigit) ||
      (hasCarry && lhs.m_digits[i] <= maxDigit);
  }
  if (hasCarry) {
    lhs.m_digits.push_back(1);
  }
  return lhs;
}

Integer& Integer::operator-=(Integer const& rhs) {
  if (m_isNegative == rhs.m_isNegative) {
    return subtractMagnitude(*this, rhs);
  }
  else {
    return addMagnitude(*this, rhs);
  }
}

Integer& subtractMagnitude(Integer& lhs, Integer const& rhs) {
  int compMag = compareMagnitude(lhs, rhs);
  Integer::SizeType numDigits = std::max(lhs.m_digits.size(), rhs.m_digits.size());
  bool hasBorrow = false;
  lhs.m_digits.resize(numDigits, 0);
  if (compMag == 0) {
    lhs.m_digits.clear();
    lhs.m_isNegative = false;
    return lhs;
  }
  for (Integer::SizeType i = 0; i < numDigits; ++i) {
    Integer::Digit lhsDigit = lhs.m_digits[i];
    Integer::Digit rhsDigit = i < rhs.m_digits.size() ? rhs.m_digits[i] : 0;
    
    lhs.m_digits[i] = (lhsDigit - rhsDigit) * compMag - hasBorrow;
    Integer::Digit compDigit = (compMag > 0) * lhsDigit + (compMag < 0) * rhsDigit;
    hasBorrow = (!hasBorrow && lhs.m_digits[i] > compDigit) ||
      (hasBorrow && lhs.m_digits[i] >= compDigit);
  }
  if (compMag < 0) {
    lhs.m_isNegative = !lhs.m_isNegative;
  }
  makeValid(lhs);
  return lhs;
}

Integer& Integer::operator*=(Integer const& rhs) {
  return multiply(Integer(*this), rhs, *this);
}

Integer aprn::operator*(Integer const& lhs, Integer const& rhs) {
  Integer result = Integer();
  return multiply(lhs, rhs, result);
}


Integer& multiply(Integer const& lhs, Integer const& rhs, Integer& result_out) {
  Integer::Digit carry = 0;
  
  result_out.m_digits.clear();
  result_out.m_isNegative = false;
  
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
    makeValid(nextSum);
    result_out += nextSum;
  }
  
  result_out.m_isNegative = lhs.m_isNegative ^ rhs.m_isNegative;
  
  makeValid(result_out);
  
  return result_out;
}

Integer& Integer::operator/=(Integer const& rhs) {
  Integer rem = Integer();
  quotRem(Integer(*this), rhs, *this, rem);
  return *this;
}

Integer aprn::operator/(Integer const& lhs, Integer const& rhs) {
  Integer quot = Integer();
  Integer rem = Integer();
  quotRem(lhs, rhs, quot, rem);
  return quot;
}

Integer& Integer::operator%=(Integer const& rhs) {
  Integer quot = Integer();
  quotRem(Integer(*this), rhs, quot, *this);
  return *this;
}

Integer aprn::operator%(Integer const& lhs, Integer const& rhs) {
  Integer quot = Integer();
  Integer rem = Integer();
  quotRem(lhs, rhs, quot, rem);
  return rem;
}

bool aprn::quotRem(Integer const& lhs, Integer const& rhs, Integer& quot_out, Integer& rem_out) {
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
  
  makeValid(quot_out);
  makeValid(rem_out);
  
  if (quotSign == -1) {
    quot_out.negate();
  }
  if (lhsSign == -1) {
    rem_out.negate();
  }
  return true;
}

bool aprn::operator==(Integer const& lhs, Integer const& rhs) {
  if (lhs.m_isNegative != rhs.m_isNegative) {
    return false;
  }
  else {
    return compareMagnitude(lhs, rhs) == 0;
  }
}

bool aprn::operator<(Integer const& lhs, Integer const& rhs) {
  if (lhs.m_isNegative != rhs.m_isNegative) {
    return lhs.m_isNegative;
  }
  else {
    return compareMagnitude(lhs, rhs) == -signum(lhs);
  }
}

int compareMagnitude(Integer const& lhs, Integer const& rhs) {
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

std::ostream& aprn::operator<<(std::ostream& os, Integer const& obj) {
  int base = 10;
  int sign = signum(obj);
  std::string digits = "0123456789";
  
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
  else if (base == 10) {
    std::string output = sign < 0 ? "-" : (os.flags() & std::ios::showpos ? "+" : "");
    Integer value = sign < 0 ? -obj : obj;
    while (true) {
      if (value < Integer(base)) {
        output.push_back(digits[value.m_digits.front()]);
        break;
      }
      else {
        Integer quotient, remainder;
        quotRem(value, base, quotient, remainder);
        value = quotient;
        output.push_back(digits[remainder.m_digits.front()]);
      }
    }
    os << output;
  }
  else {
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
  
  return os;
}

std::istream& aprn::operator>>(std::istream& is, Integer& obj) {
  if (/* no valid integer */true) {
    is.setstate(std::ios::failbit);
  }
  return is;
}

