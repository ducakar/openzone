/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

namespace detail
{

  /**
   * The common type used internally when multiplying or dividing by a scalar.
   */
  template <typename Scalar>
  struct DurationCommonType
  {
    /**
     * Internal type for integers is int64.
     */
    using Type = int64;
  };

  /**
   * Specialisation for floating-point values.
   */
  template <>
  struct DurationCommonType<float>
  {
    /**
     * Internal type for floating-point values is long double.
     */
    using Type = long double;
  };

  /**
   * Specialisation for floating-point values.
   */
  template <>
  struct DurationCommonType<double>
  {
    /**
     * Internal type for floating-point values is long double.
     */
    using Type = long double;
  };

  /**
   * Specialisation for floating-point values.
   */
  template <>
  struct DurationCommonType<long double>
  {
    /**
     * Internal type for floating-point values is long double.
     */
    using Type = long double;
  };

}

/**
 * %Time interval.
 *
 * The time interval is internally stored in nanoseconds (64-bit signed integer).
 *
 * @sa `oz::Instant`
 */
class Duration
{
public:

  /**
   * Zero time duration.
   */
  static const Duration ZERO;

private:

  int64 ns_ = 0; ///< Nanoseconds.

public:

  /**
   * Create a zero duration.
   */
  OZ_ALWAYS_INLINE
  constexpr Duration() = default;

  /**
   * Create duration for a given number of nanoseconds.
   */
  OZ_ALWAYS_INLINE
  explicit constexpr Duration(int64 ns)
    : ns_(ns)
  {}

  /**
   * True iff equally long (equal number of microseconds).
   */
  OZ_ALWAYS_INLINE
  constexpr bool operator==(const Duration& other) const
  {
    return ns_ == other.ns_;
  }

  /**
   * True iff shorter.
   */
  OZ_ALWAYS_INLINE
  constexpr bool operator<(const Duration& other) const
  {
    return ns_ < other.ns_;
  }

  /**
   * Length in nanoseconds.
   */
  OZ_ALWAYS_INLINE
  constexpr int64 ns() const
  {
    return ns_;
  }

  /**
   * Length in microseconds (integer part).
   */
  OZ_ALWAYS_INLINE
  constexpr int64 us() const
  {
    return ns_ / 1'000;
  }

  /**
   * Length in milliseconds (integer part).
   */
  OZ_ALWAYS_INLINE
  constexpr int64 ms() const
  {
    return ns_ / 1'000'000;
  }

  /**
   * Length in seconds (integer part).
   */
  OZ_ALWAYS_INLINE
  constexpr int64 s() const
  {
    return ns_ / 1'000'000'000;
  }

  /**
   * Length in seconds.
   */
  OZ_ALWAYS_INLINE
  constexpr float t() const
  {
    return float(ns_) / 1'000'000'000.0f;
  }

  /**
   * Original duration.
   */
  OZ_ALWAYS_INLINE
  constexpr Duration operator+() const
  {
    return *this;
  }

  /**
   * Negated duration.
   */
  OZ_ALWAYS_INLINE
  constexpr Duration operator-() const
  {
    return Duration(-ns_);
  }

  /**
   * Sum of durations.
   */
  OZ_ALWAYS_INLINE
  constexpr Duration operator+(const Duration& d) const
  {
    return Duration(ns_ + d.ns_);
  }

  /**
   * Difference of durations.
   */
  OZ_ALWAYS_INLINE
  constexpr Duration operator-(const Duration& d) const
  {
    return Duration(ns_ - d.ns_);
  }

  /**
   * Product of a duration and a scalar.
   */
  template <typename Scalar>
  OZ_ALWAYS_INLINE
  constexpr Duration operator*(Scalar s) const
  {
    using CommonType = typename detail::DurationCommonType<Scalar>::Type;
    return Duration(int64(CommonType(ns_) * CommonType(s)));
  }

  /**
   * Product of a scalar and a duration.
   */
  template <typename Scalar>
  OZ_ALWAYS_INLINE
  friend constexpr Duration operator*(Scalar s, const Duration& d)
  {
    return d.operator*(s);
  }

  /**
   * Duration divided by a scalar.
   */
  template <typename Scalar>
  OZ_ALWAYS_INLINE
  constexpr Duration operator/(Scalar s) const
  {
    using CommonType = typename detail::DurationCommonType<Scalar>::Type;
    return Duration(int64(CommonType(ns_) / CommonType(s)));
  }

  /**
   * Divide the duration by another duration.
   */
  OZ_ALWAYS_INLINE
  constexpr int64 operator/(const Duration& d) const
  {
    return ns_ / d.ns_;
  }

  /**
   * Remainder of division by another duration.
   */
  OZ_ALWAYS_INLINE
  constexpr Duration operator%(const Duration& d) const
  {
    return Duration(ns_ % d.ns_);
  }

  /**
   * Add a duration.
   */
  OZ_ALWAYS_INLINE
  constexpr Duration& operator+=(const Duration& d)
  {
    ns_ += d.ns_;
    return *this;
  }

  /**
   * Subtract a duration.
   */
  OZ_ALWAYS_INLINE
  constexpr Duration& operator-=(const Duration& d)
  {
    ns_ -= d.ns_;
    return *this;
  }

  /**
   * Multiply the duration by a scalar.
   */
  template <typename Scalar>
  OZ_ALWAYS_INLINE
  constexpr Duration& operator*=(Scalar s)
  {
    *this = this->operator*(s);
    return *this;
  }

  /**
   * Divide the duration by a scalar.
   */
  template <typename Scalar>
  OZ_ALWAYS_INLINE
  constexpr Duration& operator/=(Scalar s)
  {
    *this = this->operator/(s);
    return *this;
  }

};

/**
 * Create a duration for a given number of seconds.
 */
OZ_ALWAYS_INLINE
inline constexpr Duration operator""_s(unsigned long long value)
{
  return Duration(int64(value * 1'000'000'000));
}

/**
 * Create a duration for a given number of seconds.
 */
OZ_ALWAYS_INLINE
inline constexpr Duration operator""_s(long double value)
{
  return Duration(int64(value * 1'000'000'000));
}

/**
 * Create a duration for a given number of milliseconds.
 */
OZ_ALWAYS_INLINE
inline constexpr Duration operator""_ms(unsigned long long value)
{
  return Duration(int64(value * 1'000'000));
}

/**
 * Create a duration for a given number of milliseconds.
 */
OZ_ALWAYS_INLINE
inline constexpr Duration operator""_ms(long double value)
{
  return Duration(int64(value * 1'000'000));
}

/**
 * Create a duration for a given number of microseconds.
 */
OZ_ALWAYS_INLINE
inline constexpr Duration operator""_us(unsigned long long value)
{
  return Duration(int64(value * 1'000));
}

/**
 * Create a duration for a given number of microseconds.
 */
OZ_ALWAYS_INLINE
inline constexpr Duration operator""_us(long double value)
{
  return Duration(int64(value * 1'000));
}

/**
 * Create a duration for a given number of nanoseconds.
 */
OZ_ALWAYS_INLINE
inline constexpr Duration operator""_ns(unsigned long long value)
{
  return Duration(int64(value));
}

/**
 * Create a duration for a given number of nanoseconds.
 */
OZ_ALWAYS_INLINE
inline constexpr Duration operator""_ns(long double value)
{
  return Duration(int64(value));
}

}
