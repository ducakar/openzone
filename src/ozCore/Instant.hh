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
 * @file ozCore/Instant.hh
 *
 * `Instant` class.
 */

#pragma once

#include "String.hh"
#include "Duration.hh"

namespace oz
{

/**
 * %Time instant.
 *
 * The time is internally stored as nanoseconds (64-bit signed integer) from the UNIX epoch
 * (1970-01-01 00:00:00 UTC).
 *
 * @sa `oz::Duration`
 */
class Instant : private Duration
{
public:

  /**
   * Epoch.
   */
  static const Instant EPOCH;

public:

  using Duration::t;
  using Duration::s;
  using Duration::ms;
  using Duration::us;
  using Duration::ns;

  /**
   * Epoch.
   */
  Instant() = default;

  /**
   * Create instant for a given nanosecond offset from the epoch.
   */
  explicit Instant(int64 ns)
    : Duration(ns)
  {}

  /**
   * True iff equally long (equal number of microseconds).
   */
  constexpr bool operator==(const Instant& other) const
  {
    return Duration::operator==(other);
  }

  /**
   * True iff shorter.
   */
  constexpr bool operator<(const Instant& other) const
  {
    return Duration::operator<(other);
  }

  /**
   * Sum of the instant and a durations.
   */
  constexpr Instant operator+(const Duration& d) const
  {
    return static_cast<Instant&&>(Duration::operator+(d));
  }

  /**
   * Difference of the instant and a durations.
   */
  constexpr Instant operator-(const Duration& d) const
  {
    return static_cast<Instant&&>(Duration::operator-(d));
  }

  /**
   * Difference of instants.
   */
  constexpr Duration operator-(const Instant& d) const
  {
    return Duration::operator-(d);
  }

  /**
   * Add a duration.
   */
  constexpr Instant& operator+=(const Duration& d)
  {
    return static_cast<Instant&>(Duration::operator+=(d));
  }

  /**
   * Subtract a duration.
   */
  constexpr Instant& operator-=(const Duration& d)
  {
    return static_cast<Instant&>(Duration::operator-=(d));
  }

  /**
   * Monotonic clock from an unspecified point in time.
   */
  static Instant now();

};

}
