#include "../include/integer.h"

#include <algorithm>
#include <iomanip>
#include <istream>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

using namespace aprn;

void makeValid(Integer& val);

Integer::Digit const Integer::MAX_DIGIT = std::numeric_limits<Digit>::max();
Integer::Digit const Integer::CRITICAL_DIGIT = MAX_DIGIT / 2 + 1;

Integer::Integer() {
  m_digits.push_back(0);
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
  Digit* valPtr = reinterpret_cast<Digit*>(&val);
  m_digits.resize(bytes / bytesPerDigit);
  for (SizeType i = 0; i < m_digits.size(); ++i) {
    m_digits[i] = valPtr[i];
  }
  makeValid(*this);
}

Integer::Integer(unsigned long long val) {
  int bytes = sizeof(val);
  int bytesPerDigit = sizeof(Digit);
  Digit* valPtr = reinterpret_cast<Digit*>(&val);
  m_digits.resize(bytes / bytesPerDigit);
  for (SizeType i = 0; i < m_digits.size(); ++i) {
    m_digits[i] = valPtr[i];
  }
  if (m_digits.back() >= CRITICAL_DIGIT) {
    m_digits.push_back(0);
  }
  makeValid(*this);
}

// Takes an integer in invalid form and makes it valid.
void makeValid(Integer& val) {
  bool validForm = false;
  while(!validForm && val.m_digits.size() >= 2) {
    if (
        val.m_digits.back() == Integer::MAX_DIGIT &&
        val.m_digits[val.m_digits.size() - 2] >= Integer::CRITICAL_DIGIT) {
      val.m_digits.pop_back();
    }
    else if (
             val.m_digits.back() == 0 &&
             val.m_digits[val.m_digits.size() - 2] < Integer::CRITICAL_DIGIT) {
      val.m_digits.pop_back();
    }
    else {
      validForm = true;
    }
  }
}

int aprn::signum(Integer const& val) {
  if (val.m_digits.size() == 1 && val.m_digits.front() == 0) {
    return 0;
  }
  else {
    return
      (val.m_digits.back() < Integer::CRITICAL_DIGIT) -
      (val.m_digits.back() >= Integer::CRITICAL_DIGIT);
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
      lastBorrowIndex == m_digits.size() - 2 &&
      m_digits.back() == 0 &&
      m_digits[m_digits.size() - 2] == CRITICAL_DIGIT - 1) {
    m_digits.pop_back();
  }
  return *this;
}

Integer& Integer::operator+=(Integer const& rhs) {
  SizeType numDigits = std::max(m_digits.size(), rhs.m_digits.size());
  bool lhsIsNegative = m_digits.back() >= CRITICAL_DIGIT;
  bool rhsIsNegative = rhs.m_digits.back() >= CRITICAL_DIGIT;
  bool hasCarry = false;
  
  m_digits.resize(numDigits, lhsIsNegative * MAX_DIGIT);
  
  for (SizeType i = 0; i < numDigits; ++i) {
    Digit lhsDigit = m_digits[i];
    Digit rhsDigit = i < rhs.m_digits.size() ? rhs.m_digits[i] : (rhsIsNegative * MAX_DIGIT);
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

Integer& Integer::operator*=(Integer const& rhs) {
  Integer lhs(*this);
  Digit carry = 0;
  
  m_digits.clear();
  m_digits.push_back(0);
  
  for (SizeType i = 0; i < rhs.m_digits.size(); ++i) {
    Digit rhsDigit = rhs.m_digits[i];
    Integer nextSum;
    nextSum.m_digits.resize(i, 0);
    carry = 0;
    for (SizeType j = 0; j < lhs.m_digits.size(); ++j) {
      Digit lhsDigit = lhs.m_digits[j];
      DoubleDigit digitProduct = lhsDigit * rhsDigit + carry;
      carry = reinterpret_cast<Digit*>(&digitProduct)[0];
      nextSum.m_digits.push_back(reinterpret_cast<Digit*>(&digitProduct)[1]);
    }
    nextSum.m_digits.push_back(carry);
    operator+=(nextSum);
  }
  
  makeValid(*this);
  
  return *this;
}

Integer& Integer::operator/=(Integer const& rhs) {
  Integer lhs(*this);
  Integer blank = Integer();
  quotRem(lhs, rhs, *this, blank);
  return *this;
}

Integer& Integer::operator%=(Integer const& rhs) {
  Integer lhs(*this);
  Integer blank = Integer();
  quotRem(lhs, rhs, blank, *this);
  return *this;
}

void aprn::quotRem(Integer const& lhsRef, Integer const& rhsRef, Integer& quot_out, Integer& rem_out) {
  if (lhsRef < rhsRef) {
    quot_out = Integer();
    rem_out = Integer(lhsRef);
  }
  
  int lhsSign = signum(lhsRef);
  int rhsSign = signum(rhsRef);
  int quotSign = lhsSign * rhsSign;
  Integer currentDividend;
  
  if (signum(rhsRef) == 0) {
    return;
  }
  
  quot_out.m_digits.resize(lhsRef.m_digits.size(), (quotSign < 0) * Integer::MAX_DIGIT);
  
  Integer lhs = Integer(lhsRef);
  Integer rhs = Integer(rhsRef);
  
  if (lhsSign < 0) {
    lhs.negate();
  }
  if (rhsSign < 0) {
    rhs.negate();
  }
  
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
    else if (currentDividendNumDigits >= rhsNumDigits) {
      Integer::Digit* dividendEndPtr =
        reinterpret_cast<Integer::Digit*>(currentDividend.m_digits[currentDividendNumDigits - 1]);
      Integer::DoubleDigit dividendEnd, rhsEnd;
      if (currentDividendNumDigits == rhsNumDigits) {
        dividendEnd = *dividendEndPtr;
      }
      else {
        dividendEnd = *reinterpret_cast<Integer::DoubleDigit*>(dividendEndPtr);
      }
      rhsEnd = *reinterpret_cast<Integer::Digit*>(rhs.m_digits[rhsNumDigits - 1]);
      currentQuotient = dividendEnd / (rhsEnd + 1);
      Integer quotientProduct = rhs * currentQuotient;
      
      for (Integer::Digit quotient = currentQuotient + 1; quotient <= Integer::MAX_DIGIT; ++quotient) {
        Integer nextQuotientProduct = rhs * (long long unsigned) quotient;
        if (nextQuotientProduct > currentDividend) {
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
  } while (i != 0);
  
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

bool aprn::operator==(Integer const& lhs, Integer const& rhs) {
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

bool aprn::operator<(Integer const& lhs, Integer const& rhs) {
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
  Integer::SizeType i = lhs.m_digits.size() - 1;
  do {
    --i;
    Integer::Digit lhsDigit = lhs.m_digits[i];
    Integer::Digit rhsDigit = rhs.m_digits[i];
    if (lhsDigit != rhsDigit) {
      return lhsDigit < rhsDigit;
    }
  } while (i != 0);
  
  // If we made it to here, then every digit was equal, meaning that the
  // left-hand side is not smaller than the right-hand side.
  return false;
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
