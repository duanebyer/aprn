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
    Integer& operator-=(Integer const& rhs);
    Integer& operator*=(Integer const& rhs);
    Integer& operator/=(Integer const& rhs);
    Integer& operator%=(Integer const& rhs);

    //private:

    // Implementation Details
    // ----------------------
    //   An Integer is represented by a vector of digits and a sign which can be
    // positive or negative. A non-zero integer is in valid form if it has no
    // leading digits with the value of zero. The unique representation of zero
    // is an empty digit vector with a positive sign.
    //   The type used for the digit can be any unsigned integer type.
    
    // The type used for the digits of an integer.
    using Digit = std::uint8_t;
    using DoubleDigit = std::uint16_t;
    // This constant stores the maximum value a digit can have. This is
    // equal to the base minus 1.
    Digit static const MAX_DIGIT;
    // The string of digits that represents the integer.
    std::vector<Digit> m_digits;
    // The sign of the integer.
    bool m_isNegative;
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
  Integer operator*(Integer const& lhs, Integer const& rhs);
  Integer operator/(Integer const& lhs, Integer const& rhs);
  Integer operator%(Integer const& lhs, Integer const& rhs);

}

#endif
