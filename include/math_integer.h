#ifndef __APRN_MATH_INTEGER_H_
#define __APRN_MATH_INTEGER_H_

#include "integer.h"

namespace aprn {
  
  /**
   * @struct div_result
   * @brief Stores the result of a division of one Integer by another.
   * 
   * Both the quotient and the remainder are stored. If the division fails
   * because of a divide-by-zero, then the div_result::success field is set
   * to false, and the div_result::quot and div_result::rem fields are invalid.
   */
  struct div_result {
    /// The result of the division.
    Integer quot;
    /// The remainder from the division.
    Integer rem;
    /// Whether the division succeeded or not.
    bool success;
  };
  
  /**
   * @brief Gets the sign of an Integer.
   * 
   * If the Integer is positive, then +1 is returned. If it is negative, then -1
   * is returned. Otherwise, 0 is returned.
   */
  int signum(Integer const& val);
  /// @brief Returns the absolute value of an Integer.
  Integer abs(Integer const& val);
  /// @brief Returns whether the Integer is even.
  bool even(Integer const& val);
  
  /// @brief Divides one Integer by another, and returns a div_result containing the answer.
  div_result div(Integer const& lhs, Integer const& rhs);
  /**
   * @brief Divides one Integer by a positive power of two.
   * This should be used in preference to division whenever possible.
   * @param lhs The dividend
   * @param power The divisor is 2 to the power of this value
   */
  div_result div2(Integer const& lhs, Integer::ShiftType power);
  /**
   * @brief Gets the modulus of an Integer by a positive power of two.
   * 
   * This should be used in preference to the mod operator whenever possible.
   * @param lhs The value to mod by
   * @param power The right hand side is 2 to the power of this value
   */
  Integer mod2(Integer const& lhs, Integer::ShiftType power);
  
  /**
   * @brief Gets the largest Integer which divides both of a pair of Integers.
   * 
   * The result will always be positive, regardless of the sign of the arguments.
   */
  Integer gcd(Integer a, Integer b);
  
}

#endif
