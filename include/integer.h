#ifndef __APRN_INTEGER_H_
#define __APRN_INTEGER_H_

#include <cstdint>
#include <istream>
#include <ostream>
#include <type_traits>
#include <vector>

namespace aprn {
  
  struct div_result;
  
  /**
   * @class Integer
   * @author Duane Byer
   * @brief An unbounded integer type.
   * 
   */
  class Integer {
    
    // The type used for the digits of an integer.
    using Digit = std::uint8_t;
    using DoubleDigit = std::uint16_t;
    
    // The type used for the bitwise shift operators.
    using ShiftType = unsigned long long;
    
    // Friend functions.
    friend Integer operator*(Integer const& lhs, Integer const& rhs);
    friend Integer operator/(Integer const& lhs, Integer const& rhs);
    friend Integer operator%(Integer const& lhs, Integer const& rhs);
    
    friend Integer operator>>(Integer lhs, ShiftType rhs);
    friend Integer operator<<(Integer lhs, ShiftType rhs);
    
    friend std::ostream& operator<<(std::ostream& os, Integer const& obj);
    friend std::istream& operator>>(std::istream& is, Integer& obj);
    
    friend bool operator==(Integer const& lhs, Integer const& rhs);
    friend bool operator<(Integer const& lhs, Integer const& rhs);
    
    friend int signum(Integer const& val);
    friend Integer abs(Integer val);
    friend bool even(Integer const& val);
    
    friend div_result div(Integer const& lhs, Integer const& rhs);
    friend div_result div2(Integer const& lhs, ShiftType power);
    friend Integer mod2(Integer const& lhs, ShiftType power);
    
    friend Integer gcd(Integer a, Integer b);
    
  public:
    
    /**
     * @brief Constructs an Integer with a value of zero.
     */
    Integer();
    
    /*@{*/
    /**
     * @brief Constructs an Integer to have a certain value.
     * @param val The value the Integer should have
     * These constructors provide implicit conversion from all integral types
     * to the Integer type.
     */
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
    /*@}*/
    
    /*@{*/
    /**
     * @brief Explicit narrowing conversion from Integer to an integral type.
     * 
     * If the value of the Integer is too large to be stored in the target type,
     * then any bits beyond what can be stored are truncated, and the remainder
     * is returned.
     */
    explicit inline operator bool() const {
      return m_digits.size() != 0;
    }
    explicit operator signed char() const;
    explicit operator unsigned char() const;
    explicit operator signed short() const;
    explicit operator unsigned short() const;
    explicit operator signed int() const;
    explicit operator unsigned int() const;
    explicit operator signed long() const;
    explicit operator unsigned long() const;
    explicit operator signed long long() const;
    explicit operator unsigned long long() const;
    /*@}*/
    
    /**
     * @brief Gives the negative of this Integer.
     * 
     * Alternatively, Integer::negate() can be used to get the negative of
     * this Integer in place.
     */
    Integer operator-() const;
    /**
     * @brief Negates this Integer in place.
     *
     * Alternatively, Integer::operator-() will return the negative of this
     * Integer, leaving the original unchanged.
     */
    Integer& negate();
    /*@{*/
    /**
     * @brief Returns this Integer unchanged.
     * 
     * This operator is provided for stylistic reasons, but it acts as the identity.
     */
    Integer const& operator+() const;
    Integer& operator+();
    /*@}*/
    
    /**
     * @brief Increments this Integer and returns the new value.
     */
    Integer& operator++();
    /**
     * @brief Decrements this Integer and returns the new value.
     */
    Integer& operator--();
    
    /**
     * @brief Increments this Integer and returns the old value.
     */
    Integer operator++(int) {
      Integer result(*this);
      operator++();
      return result;
    }
    
    /**
     * @brief Decrements this Integer and returns the old value.
     */
    Integer operator--(int) {
      Integer result(*this);
      operator--();
      return result;
    }
    
    /*@{*/
    /**
     * @brief Basic arithmetic operations.
     */
    Integer& operator+=(Integer const& rhs);
    Integer& operator-=(Integer const& rhs);
    Integer& operator*=(Integer const& rhs);
    Integer& operator/=(Integer const& rhs);
    Integer& operator%=(Integer const& rhs);
    /*@}*/
    
    /*@{*/
    /**
     * @brief Basic bitwise operations.
     */
    Integer operator~() const;
    Integer& operator&=(Integer const& rhs);
    Integer& operator|=(Integer const& rhs);
    Integer& operator^=(Integer const& rhs);
    
    Integer& operator>>=(ShiftType n);
    Integer& operator<<=(ShiftType n);
    /*@}*/
    
  private:
    
    // Internal functions. See source file for documentation.
    
    void makeValid();
    
    Integer& addMagnitude(Integer const& rhs);
    Integer& subtractMagnitude(Integer const& rhs);
    
    Integer& setToProduct(Integer const& lhs, Integer const& rhs);
    static bool quotRem(Integer const& lhs, Integer const& rhs,
                        Integer& quot_out, Integer& rem_out);
    Integer& shiftRight(ShiftType rhs, Integer& rem_out);
    Integer& shiftLeft(ShiftType rhs);
    
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
  
  /*@{*/
  /**
   * @brief Basic comparison operators.
   */
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
  /*@}*/
  
  /*@{*/
  /**
   * @brief Basic arithmetic operators.
   */
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
  /*@}*/
  
  /*@{*/
  /**
   * @brief Basic bitwise operators.
   */
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
  /*@}*/

  /*@{*/
  /**
   * @brief Stream input and output operators.
   */
  std::ostream& operator<<(std::ostream& os, Integer const& obj);
  std::istream& operator>>(std::istream& is, Integer& obj);
  /*@}*/
  
}

#endif
