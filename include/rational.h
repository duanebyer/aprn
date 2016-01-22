#ifndef __APRN_RATIONAL_H_
#define __APRN_RATIONAL_H_

#include <istream>
#include <ostream>

#include "integer.h"

namespace aprn {

  /**
   * @class Rational
   * @brief An arbitrary-precision rational number type.
   * @author Duane Byer
   */
  class Rational {
    
    friend std::ostream& operator<<(std::ostream& os, Rational const& obj);
    friend std::istream& operator>>(std::istream& is, Rational& obj);
    
  public:
    
    /// @brief Constructs a Rational with a value of zero.
    Rational();
    
    /*@{*/
    /**
     * @brief Constructs a Rational to have a certain value.
     * 
     * This constructor can also be used to convert any compatible
     * type to a Rational.
     */
    Rational(Integer val);
    Rational(float val);
    Rational(double val);
    Rational(long double val);
    /*@}*/
    
    /*@{*/
    /**
     * @brief Explicit narrowing conversion from Rational to another type.
     * 
     * The result will be the closest approximation to the value of the Rational
     * that can be made, truncating towards zero. In the case that the Rational
     * is larger than any possible value of the target type, an infinite value
     * will be returned.
     */
    explicit operator Integer() const;
    explicit operator float() const;
    explicit operator double() const;
    explicit operator long double() const;
    /*@}*/
    
    /// @brief Gives the negative of this Rational.
    Rational operator-() const;
    /// @brief Negates this Rational in place.
    Integer& negate();
    /*@{*/
    /// @brief Returns this Rational unchanged.
    Rational const& operator+() const {
      return *this;
    }
    Rational operator+() {
      return *this;
    }
    /*@}*/
    
    /// @brief Increments this Rational and returns the new value.
    Rational& operator++();
    
    /// @brief Decrements this Rational and returns the new value.
    Rational& operator--();
    
    /// @brief Increments this Rational and returns the old value.
    Rational operator++(int) {
      Rational result(*this);
      operator++();
      return result;
    }
    
    /// @brief Decrements this Rational and returns the old value.
    Rational operator--(int) {
      Rational result(*this);
      operator--();
      return result;
    }
    
    /// @brief Adds another Rational to this one.
    Rational& operator+=(Rational const& rhs);
    /// @brief Subtracts another Rational from this one.
    Rational& operator-=(Rational const& rhs);
    /// @brief Multiplies another Rational to this one.
    Rational& operator*=(Rational const& rhs);
    /// @brief Divides this Rational by another one.
    Rational& operator/=(Rational const& rhs);
    /// @brief Modulates this Rational by another one.
    Rational& operator%=(Rational const& rhs);
    
  private:
    
    // Internal functions. See source file for documentation.
    
    void makeValid();
    
    // Implementation Details
    //-----------------------
    //   A Rational is represented as a fraction of two integers: the numerator
    // and the denominator. Every Rational has a unique representation where
    // the numerator and the denominator share no common factors other than
    // one and the denominator is positive. The unique representation of zero
    // is when the numerator is zero and the denominator is one.
    
    Integer m_numerator;
    Integer m_denominator;
    
  };
  
  /// @brief Checks if this Rational equals another one.
  bool operator==(Rational const& lhs, Rational const& rhs);
  /// @brief Checks if this Rational is smaller than another one.
  bool operator<(Rational const& lhs, Rational const& rhs);
  
  /// @brief Checks if this Rational does not equal another one.
  inline bool operator!=(Rational const& lhs, Rational const& rhs) {
    return !operator==(lhs, rhs);
  }
  /// @brief Checks if this Rational is greater than another one.
  inline bool operator>(Rational const& lhs, Rational const& rhs) {
    return operator<(rhs, lhs);
  }
  /// @brief Checks if this Rational is smaller than or equal to another one.
  inline bool operator<=(Rational const& lhs, Rational const& rhs) {
    return !operator>(lhs, rhs);
  }
  /// @brief Checks if this Rational is greater than or equal to another one.
  inline bool operator>=(Rational const& lhs, Rational const& rhs) {
    return !operator<(lhs, rhs);
  }
  
  /// @brief Returns the sum of two Rationals.
  inline Rational operator+(Rational lhs, Rational const& rhs) {
    lhs += rhs;
    return lhs;
  }
  /// @brief Returns the difference of two Rationals.
  inline Rational operator-(Rational lhs, Rational const& rhs) {
    lhs -= rhs;
    return lhs;
  }
  /// @brief Returns the product of two Rationals.
  inline Rational operator*(Rational lhs, Rational const& rhs) {
    lhs *= rhs;
    return lhs;
  }
  /// @brief Returns the quotient of two Rationals.
  inline Rational operator/(Rational lhs, Rational const& rhs) {
    lhs /= rhs;
    return lhs;
  }
  
  /// @brief Outputs a Rational to a standard stream.
  std::ostream& operator<<(std::ostream& os, Rational const& obj);
  /// @brief Reads in a Rational from a standard stream.
  std::istream& operator>>(std::istream& is, Rational& obj);

}

#endif
