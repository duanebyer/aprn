#ifndef __APRN_INTEGER_H_
#define __APRN_INTEGER_H_

#include <cstdint>
#include <istream>
#include <ostream>
#include <vector>

namespace aprn {

  class Integer {
    
    // The type used for the digits of an integer.
    using Digit = std::uint8_t;
    using DoubleDigit = std::uint16_t;
    
    // The type used for the bitwise shift operators.
    using ShiftType = unsigned long long;
    
    friend Integer operator*(Integer const& lhs, Integer const& rhs);
    friend Integer operator/(Integer const& lhs, Integer const& rhs);
    friend Integer operator%(Integer const& lhs, Integer const& rhs);
    
    friend Integer operator>>(Integer lhs, ShiftType rhs);
    friend Integer operator<<(Integer lhs, ShiftType rhs);
    
    friend std::ostream& operator<<(std::ostream& os, Integer const& obj);
    friend std::istream& operator>>(std::istream& is, Integer& obj);
    
    friend int signum(Integer const& val);
    friend bool quotRem(Integer const& lhs, Integer const& rhs, Integer& quot_out, Integer& rem_out);
    friend bool operator==(Integer const& lhs, Integer const& rhs);
    friend bool operator<(Integer const& lhs, Integer const& rhs);
    
  public:
    
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
    Integer const& operator+() const;
    Integer& operator+();
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
    
    Integer operator~() const;
    Integer& operator&=(Integer const& rhs);
    Integer& operator|=(Integer const& rhs);
    Integer& operator^=(Integer const& rhs);
    
    Integer& operator>>=(ShiftType n);
    Integer& operator<<=(ShiftType n);
    
    
  private:
    
    void makeValid();
    
    Integer& addMagnitude(Integer const& rhs);
    Integer& subtractMagnitude(Integer const& rhs);
    
    Integer& setToProduct(Integer const& lhs, Integer const& rhs);
    static bool quotRem(Integer const& lhs, Integer const& rhs, Integer& quot_out, Integer& rem_out);
    
    static int compareMagnitude(Integer const& lhs, Integer const& rhs);
    
    // Implementation Details
    // ----------------------
    //   An Integer is represented by a vector of digits and a sign which can be
    // positive or negative. A non-zero integer is in valid form if it has no
    // leading digits with the value of zero. The unique representation of zero
    // is an empty digit vector with a positive sign.
    //   The type used for the digit can be any unsigned integer type.
    
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
  bool quotRem(Integer const& lhs, Integer const& rhs, Integer& quot_out, Integer& rem_out);
  
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
  
  inline Integer operator&(Integer lhs, Integer const& rhs) {
    lhs &= rhs;
    return lhs;
  }
  inline Integer operator|(Integer lhs, Integer const& rhs) {
    lhs |= rhs;
    return lhs;
  }
  inline Integer operator>>(Integer lhs, Integer::ShiftType rhs) {
    lhs >>= rhs;
    return lhs;
  }
  inline Integer operator<<(Integer lhs, Integer::ShiftType rhs) {
    lhs <<= rhs;
    return lhs;
  }
  
  std::ostream& operator<<(std::ostream& os, Integer const& obj);
  std::istream& operator>>(std::istream& is, Integer& obj);
  
}

#endif
