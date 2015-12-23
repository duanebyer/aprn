#ifndef __APRN_INTEGER_H_
#define __APRN_INTEGER_H_

#include <algorithm>
#include <limits>
#include <vector>

namespace aprn {

  class Integer {

    friend int signum(Integer const& val);
    friend bool operator==(Integer const& lhs, Integer const& rhs);
    friend bool operator<(Integer const& lhs, Integer const& rhs);

  public:
    
    // TODO: unary plus operator
    
    Integer operator-() {
      Integer result(*this);
      return result.negate();
    }

    Integer& negate() {
      for (SizeType i = 0; i < m_digits.size(); ++i) {
        m_digits[i] = ~m_digits[i];
      }
      operator++();
      return *this;      
    }

    Integer& operator++() {
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
      if (lastCarryIndex == m_digits.size() - 2 && m_digits.back() == MAX_DIGIT && m_digits[m_digits.size() - 2] == CRITICAL_DIGIT) {
        m_digits.pop_back();
      }
      return *this;
    }

    Integer operator++(int) {
      Integer result(*this);
      operator++();
      return result;
    }

    Integer& operator--() {
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
      if (lastCarryIndex = m_digits.size() - 2 && m_digits.back() == 0 && m_digits[m_digits.size() - 2] == CRITICAL_DIGIT - 1) {
        m_digits.pop_back();
      }
      return *this;
    }

    Integer operator--(int) {
      Integer result(*this);
      operator--();
      return result;
    }

    Integer& operator+=(Integer const& rhs) {
      SizeType numDigits = std::max(m_digits.size(), rhs.m_digits.size());
      m_digits.resize(numDigits);
      rhs.m_digits.resize(numDigits);
      
      for (SizeType i = 0; i < numDigits; ++i) {
        
      }
      
      return *this;
    }

    Integer& operator-=(Integer const& rhs) {
      return *this;
    }
    
    Integer& operator*=(Integer const& rhs) {
      return *this;
    }
    
    Integer& operator/=(Integer const& rhs) {
      return *this;
    }

  private:

    // Implementation Details
    // ----------------------
    //   An Integer is represented by a vector of digits in complement form (similar
    // to two's complement, but for an arbitrary power-of-two base). If the most
    // significant digit is smaller than half of the base, the integer is positive,
    // otherwise it is negative. If there are no digits at all, the integer is zero.
    // To negate an integer, each digit must be replaced with its complement, and
    // one must be added to the result.
    //   Each integer is guaranteed to have a unique representation as a string of
    // digits satisfying the following constraints (where b is the base).
    //   1) The representation of '0' is the single digit 0. All other strings that
    //      would evaluate to 0 are invalid.
    //        invalid base-8 examples: '000', '70'
    //        fixed base-8 examples:     '0',  '0'
    //   2) If the most significant digit is a 0, then the second-most significant
    //      digit must be larger or equal to b/2.
    //        invalid base-8 examples: '03', '017', '023', '004', '0005'
    //        fixed base-8 examples:    '3',  '17',  '23',  '04',   '05'
    //   3) If the most significant digit is b-1, then the second-most significant
    //      digit must be smaller than b/2.
    //        invalid base-8 examples: '75', '763', '772', '7771'
    //        fixed base-8 examples:    '5',  '63',  '72',   '71'
    //
    //   The type used for the digit can be any unsigned integer type.
    
    // The type used for the digits of an integer.
    using Digit = unsigned int;
    // This constant stores the maximum value a digit can have. This is
    // equal to the base minus 1.
    Digit static const MAX_DIGIT = std::numeric_limits<Digit>::max();
    // This constant stores the particular value such that if the most
    // significant digit is larger than this value, the integer is
    // negative, and if it is smaller, then the integer is positive.
    Digit static const CRITICAL_DIGIT = MAX_DIGIT / 2;
    // The string of digits that represents the integer.
    std::vector<Digit> m_digits;
    // The size type of the vector.
    using SizeType = decltype(m_digits)::size_type;

  };

  int signum(Integer const& val) {
    if (val.m_digits.size() == 1 && val.m_digits.front() == 0) {
      return 0;
    }
    else {
      return
        (val.m_digits.back() < Integer::CRITICAL_DIGIT) -
        (val.m_digits.back() > Integer::CRITICAL_DIGIT);
    }
  }

  // TODO: stream operators

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

  inline bool operator!=(Integer const& lhs, Integer const& rhs) {
    return !operator==(lhs, rhs);
  }
  inline bool operator>(Integer const& lhs, Integer const& rhs) {
    return operator<(rhs, lhs);
  }
  inline bool operator<=(Integer const& lhs, Integer const& rhs) {
    return !operator>(lhs, rhs);
  }
  inline bool operator>=(Integer const& lhs, Integer const& rhs) {
    return !operator<(lhs, rhs);
  }

  inline Integer operator+(Integer lhs, Integer const& rhs) {
    lhs += rhs;
    return lhs;
  }
  inline Integer operator-(Integer lhs, Integer const& rhs) {
    lhs -= rhs;
    return lhs;
  }
  inline Integer operator*(Integer lhs, Integer const& rhs) {
    lhs *= rhs;
    return lhs;
  }
  inline Integer operator/(Integer lhs, Integer const& rhs) {
    lhs /= rhs;
    return lhs;
  }

}

#endif
