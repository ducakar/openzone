/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2016 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file ozCore/Duration.hh
 *
 * `Duration` class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * %Time duration class.
 *
 * A time interval or a time relative to an unspecified epoch (as returned by `Time::clock()`).
 * The time interval is internally stored in nanoseconds (64-bit integer), hence nanosecond
 * resolution.
 */
class Duration
{
public:

  /**
   * Zero time duration.
   */
  static const Duration ZERO;

private:

  long64 ns_ = 0; ///< Nanoseconds.

public:

  /**
   * Create a zero duration.
   */
  constexpr Duration() = default;

  /**
   * Create duration for a given number of nanoseconds.
   */
  explicit constexpr Duration(long64 ns)
    : ns_(ns)
  {}

  /**
   * True iff equally long (equal number of microseconds).
   */
  constexpr bool operator==(const Duration& d) const
  {
    return ns_ == d.ns_;
  }

  /**
   * True iff not qually long.
   */
  constexpr bool operator!=(const Duration& d) const
  {
    return !operator==(d);
  }

  /**
   * True iff shorter.
   */
  constexpr bool operator<(const Duration& d) const
  {
    return ns_ < d.ns_;
  }

  /**
   * True iff equally long or shorted.
   */
  constexpr bool operator<=(const Duration& d) const
  {
    return ns_ <= d.ns_;
  }

  /**
   * True iff longer.
   */
  constexpr bool operator>(const Duration& d) const
  {
    return !operator<=(d);
  }

  /**
   * True iff equally long or longer.
   */
  constexpr bool operator>=(const Duration& d) const
  {
    return !operator<(d);
  }

  /**
   * Length in seconds.
   */
  constexpr float sf() const
  {
    return float(ns_) / 1000000000.0f;
  }

  /**
   * Length in seconds (integer part).
   */
  constexpr long64 s() const
  {
    return ns_ / 1000000000;
  }

  /**
   * Length in milliseconds (integer part).
   */
  constexpr long64 ms() const
  {
    return ns_ / 1000000;
  }

  /**
   * Length in microseconds (integer part).
   */
  constexpr long64 us() const
  {
    return ns_ / 1000;
  }

  /**
   * Length in nanoseconds.
   */
  constexpr long64 ns() const
  {
    return ns_;
  }

  /**
   * Original duration.
   */
  constexpr Duration operator+() const
  {
    return *this;
  }

  /**
   * Negated duration.
   */
  constexpr Duration operator-() const
  {
    return Duration(-ns_);
  }

  /**
   * Sum of durations.
   */
  constexpr Duration operator+(const Duration& d) const
  {
    return Duration(ns_ + d.ns_);
  }

  /**
   * Difference of durations.
   */
  constexpr Duration operator-(const Duration& d) const
  {
    return Duration(ns_ - d.ns_);
  }

  /**
   * Product of a duration and a scalar.
   */
  template <typename Scalar = long64>
  constexpr Duration operator*(Scalar s) const
  {
    return Duration(long64(ns_ * s));
  }

  /**
   * Product of a scalar and a duration.
   */
  template <typename Scalar = long64>
  friend constexpr Duration operator*(Scalar s, const Duration& d)
  {
    return Duration(long64(s * d.ns_));
  }

  /**
   * Duration divided by a scalar.
   */
  template <typename Scalar = long64>
  constexpr Duration operator/(Scalar s) const
  {
    return Duration(long64(ns_ / s));
  }

  /**
   * Divide the duration by anoter duration.
   */
  constexpr long64 operator/(const Duration& d) const
  {
    return ns_ / d.ns_;
  }

  /**
   * Remainder of division by anoter duration.
   */
  constexpr Duration operator%(const Duration& d) const
  {
    return Duration(ns_ % d.ns_);
  }

  /**
   * Add a duration.
   */
  constexpr Duration& operator+=(const Duration& d)
  {
    ns_ += d.ns_;
    return *this;
  }

  /**
   * Subtract a duration.
   */
  constexpr Duration& operator-=(const Duration& d)
  {
    ns_ -= d.ns_;
    return *this;
  }

  /**
   * Multiply the duration by a scalar.
   */
  template <typename Scalar = long64>
  constexpr Duration& operator*=(Scalar s)
  {
    ns_ = long64(ns_ * s);
    return *this;
  }

  /**
   * Divide the duration by a scalar.
   */
  template <typename Scalar = long64>
  constexpr Duration& operator/=(Scalar s)
  {
    ns_ = long64(ns_ / s);
    return *this;
  }

};

/**
 * Create a duration for a given number of seconds.
 */
inline constexpr Duration operator""_s(ulong64 value)
{
  return Duration(long64(value * 1000000000));
}

/**
 * Create a duration for a given number of milliseconds.
 */
inline constexpr Duration operator""_ms(ulong64 value)
{
  return Duration(long64(value * 1000000));
}

/**
 * Create a duration for a given number of microseconds.
 */
inline constexpr Duration operator""_us(ulong64 value)
{
  return Duration(long64(value * 1000));
}

/**
 * Create a duration for a given number of nanoseconds.
 */
inline constexpr Duration operator""_ns(ulong64 value)
{
  return Duration(long64(value));
}

}
