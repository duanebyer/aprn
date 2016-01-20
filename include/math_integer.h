#ifndef __APRN_MATH_INTEGER_H_
#define __APRN_MATH_INTEGER_H_

#include "integer.h"

namespace aprn {
  
  struct div_result {
    Integer quot;
    Integer rem;
    bool success;
  };
  
  int signum(Integer const& val);
  Integer abs(Integer const& val);
  bool even(Integer const& val);
  
  div_result div(Integer const& lhs, Integer const& rhs);
  div_result div2(Integer const& lhs, Integer::ShiftType power);
  Integer mod2(Integer const& lhs, Integer::ShiftType power);
  
  Integer gcd(Integer a, Integer b);
  
}

#endif
