#include "include/integer.h"

#include <algorithm>
#include <istream>
#include <limits>
#include <ostream>
#include <string>
#include <vector>

using namespace aprn;

Digit const Integer::MAX_DIGIT = std::numeric_limits<Digit>::max();
Digit const Integer::CRITICAL_DIGIT = MAX_DIGIT / 2;

Integer::Integer(signed long long val) {
  int bytes = sizeof(val);
  int bytesPerDigit = sizeof(Digit);
  Digit* valPtr = m_digits[i] = *reinterpret_cast<Digit*>(&val);
  m_digits.resize(bytes / bytesPerDigit);
  for (int i = 0; i < m_digits.size(); ++i) {
    m_digits[i] = valPtr[i];
  }
}

Integer::Integer(unsigned long long val) {
  int bytes = sizeof(val);
  int bytesPerDigit = sizeof(Digit);
  Digit* valPtr = m_digits[i] = *reinterpret_cast<Digit*>(&val);
  m_digits.resize(bytes / bytesPerDigit);
  for (int i = 0; i < m_digits.size(); ++i) {
    m_digits[i] = valPtr[i];
  }
  if (m_digits.last() >= CRITICAL_DIGIT) {
    m_digits.push_back(0);
  }
}

// Takes an integer in invalid form and makes it valid.
void makeValid(Integer& val) {
  bool validForm = false;
  while(!validForm && val.m_digits.size() >= 2) {
    if (
        val.m_digits.last() == MAX_DIGIT &&
        val.m_digits[m_digits.size() - 2] >= CRITICAL_DIGIT) {
      valm_digits.pop_back();
    }
    else if (
             val.m_digits.last() == 0 &&
             val.m_digits[m_digits.size() - 2] < CRITICAL_DIGIT) {
      val.m_digits.pop_back();
    }
    else {
      validForm = true;
    }
  }
}

Integer Integer::operator-() const {
  Integer result(*this);
  return result.negate();
}

Integer& Integer::negate() {
  for (SizeType i = 0; i < m_digits.size(); ++i) {
    m_digits[i] = ~m_digits[i];
  }
  operator++();
  return *this;      
}

Integer& Integer::operator++() {
  bool hasCarry = false;
  SizeType lastCarryIndex;
  for (SizeType i = 0; i < m_digits.size(); ++i) {
    hasCarry = (++m_digits[i] == 0);
    lastCarryIndex = i;
    if (!hasCarry) {
      break;
    }
  }
  if (lastCarryIndex == m_digits.size() - 1 && m_digits.back() == CRITICAL_DIGIT) {
    m_digits.push_back(0);
  }
  if (
      lastCarryIndex == m_digits.size() - 2 &&
      m_digits.back() == MAX_DIGIT &&
      m_digits[m_digits.size() - 2] == CRITICAL_DIGIT) {
    m_digits.pop_back();
  }
  return *this;
}

Integer& Integer::operator--() {
  bool hasBorrow = false;
  SizeType lastBorrowIndex;
  for (SizeType i = 0; i < m_digits.size(); ++i) {
    hasBorrow = (--m_digits[i] == MAX_DIGIT);
    lastBorrowIndex = i;
    if (!hasBorrow) {
      break;
    }
  }
  if (lastBorrowIndex == m_digits.size() - 1 && m_digits.back() == CRITICAL_DIGIT - 1) {
    m_digits.push_back(MAX_DIGIT);
  }
  if (
      lastCarryIndex = m_digits.size() - 2 &&
      m_digits.back() == 0 &&
      m_digits[m_digits.size() - 2] == CRITICAL_DIGIT - 1) {
    m_digits.pop_back();
  }
  return *this;
}

Integer& Integer::operator+=(Integer const& rhs) {
  SizeType numDigits = std::max(m_digits.size(), rhs.m_digits.size());
  bool lhsIsNegative = m_digits.last() >= CRITICAL_DIGIT;
  bool rhsIsNegative = rhs.m_digits.last() >= CRITICAL_DIGIT;
  bool hasCarry = false;
  
  m_digits.resize(numDigits, isNegative * MAX_DIGIT);
  
  for (SizeType i = 0; i < numDigits; ++i) {
    Digit lhsDigit = m_digits[i];
    Digit rhsDigit = i < rhs.m_digits.size() ? rhs.m_digits[i] : (rhsIsNegative * MAX_DIGIT)
    Digit minDigit = std::min(m_digits[i], rhs.m_digits[i]);

    m_digits[i] += rhs.m_digits[i] + hasCarry;
    hasCarry = m_digits[i] < minDigit;
  }
  
  if (hasCarry) {
    m_digits.push_back(hasCarry + lhsIsNegative * MAX_DIGIT + rhsIsNegative * MAX_DIGIT);
  }

  makeValid(*this);

  return *this;
}

Integer& operator*=(Integer const& rhs) {
  Integer lhs(*this);
  Digit carry = 0;
  
  m_digits.clear();
  m_digits.push_back(0);
  
  for (SizeType i = 0; i < rhs.m_digits.size(); ++i) {
    Digit rhsDigit = rhs.m_digits[i];
    Integer nextSum;
    nextSum.resize(i, 0);
    carry = 0;
    for (SizeType j = 0; j < lhs.m_digits.size(); ++j) {
      Digit lhsDigit = lhs.m_digits[j];
      DoubleDigit digitProduct = lhsDigit * rhsDigit + carry;
      carry = reinterpret_cast<Digit*>(&digitProduct)[0];
      nextSum.push_back(reinterpret_cast<Digit*>(&digitProduct)[1]);
    }
    nextSum.push_back(carry);
    operator+=(nextSum);
  }
  
  makeValid(*this);
  
  return *this;
}

Integer& operator/=(Integer const& rhs) {
  Integer lhs(*this);
  quotRem(lhs, rhs, *this, Integer(0));
  return *this;
}

Integer& operator%=(Integer const& rhs) {
  Integer lhs(*this);
  quotRem(lhs, rhs, Integer(0), *this);
  return *this;
}

void quotRem(Integer const& lhs, Integer const& rhs, Integer& quot_out, Integer& rem_out) {
  if (lhs < rhs) {
    quot_out = Integer(0);
    rem_out = Integer(lhs);
  }
  if (rhs == 0) {
    return;
  }
  
  int lhsSign = signum(lhs);
  int rhsSign = signum(rhs);
  int quotSign = lhsSign * rhsSign;
  quot_out.m_digits.resize(lhs.m_digits.size(), (quotSign < 0) * MAX_DIGIT);
  Integer currentDividend;
  
  if (lhsSign < 0) {
    lhs = -lhs;
  }
  if (rhsSign < 0) {
    rhs = -rhs;
  }
  
  for (SizeType i = lhs.m_digits.size() - 1; i >= 0; --i) {
    currentDividend.insert(currentDividend.begin(), lhs.m_digits[i]);
    Digit currentQuotient;
    SizeType currentDividendNumDigits = currentDividend.m_digits.size();
    SizeType rhsNumDigits = rhs.m_digits.size();
    if (currentDividendNumDigits < rhsNumDigits) {
      currentQuotient = 0;
    }
    else if (currentDividendNumDigits >= rhsNumDigits) {
      Digit* dividendEndPtr = reinterpret_cast<Digit*>(currentDividend.m_digits[currentDividendNumDigits - 1]);
      DoubleDigit dividendEnd, rhsEnd;
      if (currentDividendNumDigits == rhsNumDigits) {
        dividendEnd = *dividendEndPtr;
      }
      else {
        dividendEnd = *reinterpret_cast<DoubleDigit*>(dividendEndPtr);
      }
      rhsEnd = *reinterpret_cast<Digit*>(rhs.m_digits[rhsNumDigits - 1]);
      currentQuotient = dividendEnd / (rhsEnd + 1);
      Integer quotientProduct = rhs * i;
      
      for (Digit i = currentQuotient + 1; i <= MAX_DIGIT; ++i) {
        Integer nextQuotientProduct = rhs * i;
        if (product > currentDividend) {
          break;
        }
        else {
          quotientProduct = nextQuotientProduct;
          currentQuotient = i;
        }
      }
      
      currentDividend -= quotientProduct;
    }
    
    quot_out.m_digits[i] = currentQuotient;
  }
  
  rem_out = currentDividend;
  
  makeValid(quot_out);
  makeValid(rem_out);
  
  if (quotSign == -quotSign) {
    quot_out.negate();
    if (signum(rem_out) != 0) {
      quot_out -= rhs;
    }
  }
}

bool operator==(Integer const& lhs, Integer const& rhs) {
  if (lhs.m_digits.size() != rhs.m_digits.size()) {
    return false;
  }
  for (Integer::SizeType i = 0; i < lhs.m_digits.size(); ++i) {
    Integer::Digit lhsDigit = lhs.m_digits[i];
    Integer::Digit rhsDigit = rhs.m_digits[i];
    if (lhsDigit != rhsDigit) {
      return false;
    }
  }
  return true;
}

bool operator<(Integer const& lhs, Integer const& rhs) {
  int lhsSignum = signum(lhs);
  int rhsSignum = signum(rhs);
  
  if (lhs.m_digits.size() != rhs.m_digits.size()) {
    return lhsSignum * lhs.m_digits.size() < rhsSignum * rhs.m_digits.size();
  }
  
  Integer::Digit lhsSignDigit = lhs.m_digits.back();
  Integer::Digit rhsSignDigit = rhs.m_digits.back();
    
  // The most significant digit is the sign digit, which is ordered in a
  // different order than the rest of the digits. For example, if the sign
  // digit was in base 8:   4 < 5 < 6 < 7 < 0 < 1 < 2 < 3.
  if (lhsSignDigit != rhsSignDigit) {
    // This complicated boolean expression is just comparing the sign digit
    // based on the above ordering.
    bool hasSameSign = (lhsSignum == rhsSignum);
    return
      (!hasSameSign && (lhsSignum < rhsSignum)) ||
      (hasSameSign && (lhsSignDigit < rhsSignDigit));
  }
  
  // All of the other digits are ordered in the expected order, which is:
  // 0 < 1 < 2 ... . Here, the digits are iterated through from most
  // significant to least significant.
  for (Integer::SizeType i = lhs.m_digits.size() - 2; i >= 0; --i) {
    Integer::Digit lhsDigit = lhs.m_digits[i];
    Integer::Digit rhsDigit = rhs.m_digits[i];
    if (lhsDigit != rhsDigit) {
      return lhsDigit < rhsDigit;
    }
  }
  
  // If we made it to here, then every digit was equal, meaning that the
  // left-hand side is not smaller than the right-hand side.
  return false;
}

std::ostream& operator<<(std::ostream& os, Integer const& obj) {
  int base = 10;
  int sign = signum(obj);
  Integer value = sign < 0 ? -obj : obj;
  std::string digits = "0123456789abcdef";
  
  if (os.flags() & std::ios::uppercase) {
    std::transform(digits.begin(), digits.end(), digits.begin(), ::toupper);
  }
  
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
  
  if (os.flags() & std::ios::showpos) {
    if (sign >= 0) {
      os << '+';
    }
  }
  if (sign < 0) {
    os << '-';
  }
  
  if (os.flags() & std::ios::showbase) {
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
  else {
    std::string output = "";
    while (true) {
      if (value < base) {
        output.push_back(digits[(int) value]);
        break;
      }
      else {
        Integer[] dr = divRem(value, base);
        value = dr[0];
        output.push_back(digits[(int) dr[1]]);
      }
    }
    os << output;
  }
  
  return os;
}

std::istream& operator>>(std::istream& is, Integer& obj) {
  if (/* no valid integer */true) {
    is.setstate(std::ios::failbit);
  }
  return is;
}
