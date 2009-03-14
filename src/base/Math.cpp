/*
 *  Math.cpp
 *
 *  Math constants
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "base.hpp"

#include <math.h>
#include <stdlib.h>
#include <time.h>

namespace oz
{

  const float Math::FLOAT_EPS  = 1.1920928955078125e-7f;
  const float Math::DOUBLE_EPS = 2.2204460492503130808e-16f;
  const float Math::E          = 2.7182818284590452354f;
  const float Math::LOG2E      = 1.4426950408889634074f;
  const float Math::LOG10E     = 0.43429448190325182765f;
  const float Math::LN2        = 0.69314718055994530942f;
  const float Math::LN10       = 2.30258509299404568402f;
  const float Math::PI         = 3.14159265358979323846f;
  const float Math::PI_2       = 1.57079632679489661923f;
  const float Math::PI_4       = 0.78539816339744830962f;
  const float Math::_1_PI      = 0.31830988618379067154f;
  const float Math::_2_PI      = 0.63661977236758134308f;
  const float Math::_2_SQRTPI  = 1.12837916709551257390f;
  const float Math::SQRT2      = 1.41421356237309504880f;
  const float Math::SQRT1_2    = 0.70710678118654752440f;
  const float Math::NaN        = (float) HUGE_VAL * 0.0f;
  const float Math::INF        = (float) HUGE_VAL;

  const Math Math::math;

  Math::Math()
  {
    srand( (uint) time( null ) );
  }

  int ( *const Math::rand )() = ::rand;

}
