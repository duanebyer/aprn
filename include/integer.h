#ifndef __APRN_INTEGER_H_
#define __APRN_INTEGER_H_

#include <cstdint>
#include <istream>
#include <ostream>
#include <vector>

namespace aprn {

  class Integer {

    friend std::ostream& operator<<(std::ostream& os, Integer const& obj);
    friend std::istream& operator>>(std::istream& is, Integer& obj);
    
    friend int signum(Integer const& val);
    friend void quotRem(Integer const& lhs, Integer const& rhs, Integer& quot_out, Integer& rem_out);
    friend bool operator==(Integer const& lhs, Integer const& rhs);
    friend bool operator<(Integer const& lhs, Integer const& rhs);

  public:
    
    // TODO: unary plus operator
    Integer();
    Integer(signed char val);
    Integer(unsigned char val);
    Integer(signed short val);
    Integer(unsigned short val);
    Integer(signed int val);
    Integer(unsigned int val);
    Integer(signed long val);
    Integer(unsigned long val);
    Integer(signed long long val);
    Integer(unsigned long long val);
    
    Integer operator-() const;
    Integer& negate();

    Integer& operator++();
    Integer& operator--();

    Integer operator++(int) {
      Integer result(*this);
      operator++();
      return result;
    }
    
    Integer operator--(int) {
      Integer result(*this);
      operator--();
      return result;
    }

    Integer& operator+=(Integer const& rhs);
    Integer& operator*=(Integer const& rhs);
    Integer& operator/=(Integer const& rhs);
    Integer& operator%=(Integer const& rhs);

    Integer& operator-=(Integer const& rhs) {
      operator+=(-rhs);
      return *this;
    }

    //private:

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
    using Digit = std::uint8_t;
    using DoubleDigit = std::uint16_t;
    // This constant stores the maximum value a digit can have. This is
    // equal to the base minus 1.
    Digit static const MAX_DIGIT;
    // This constant stores the particular value such that if the most
    // significant digit is larger than this value, the integer is
    // negative, and if it is smaller, then the integer is positive.
    Digit static const CRITICAL_DIGIT;
    // The string of digits that represents the integer.
    std::vector<Digit> m_digits;
    // The size type of the vector.
    using SizeType = decltype(m_digits)::size_type;

  };

  int signum(Integer const& val);
  void quotRem(Integer const& lhs, Integer const& rhs, Integer& quot_out, Integer& rem_out);
  
  std::ostream& operator<<(std::ostream& os, Integer const& obj);
  std::istream& operator>>(std::istream& is, Integer& obj);

  bool operator==(Integer const& lhs, Integer const& rhs);
  bool operator<(Integer const& lhs, Integer const& rhs);

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
  inline Integer operator%(Integer lhs, Integer const& rhs) {
    lhs %= rhs;
    return lhs;
  }

}

#endif
