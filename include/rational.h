#ifndef __APRN_RATIONAL_H_
#define __APRN_RATIONAL_H_

namespace aprn {

  /**
   * @class Rational
   * @brief An arbitrary-precision rational number type.
   * @author Duane Byer
   */
  class Rational {
    
  public:
    
    /// @brief Constructs a Rational with a value of zero.
    Rational();
    
    /*@{*/
    /**
     * @brief Constructs a Rational to have a 
     */
    Rational(Integer val);
    Rational(float val);
    Rational(double val);
    Rational(long double val);
    /*@}*/
    
  };

}

#endif
