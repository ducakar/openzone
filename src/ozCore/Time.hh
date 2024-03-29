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
 * @file ozCore/Time.hh
 *
 * `Time` class.
 */

#pragma once

#include "String.hh"

namespace oz
{

/**
 * Broken-down calendar time representation and other time-related utilities.
 */
class Time
{
public:

  int year;   ///< Year, all digits.
  int month;  ///< Month, from 1 to 12.
  int day;    ///< Day in month, from 1 to 31.
  int hour;   ///< Hour.
  int minute; ///< Minute.
  int second; ///< Second.

public:

  /**
   * Get current time in seconds from the platform-dependent epoch.
   *
   * @note
   * Epoch may differ among platforms (e.g. it is 1970-01-01 0:00:00 on Linux and 1601-01-01 0:00:00
   * on Windows).
   */
  static int64 epoch();

  /**
   * Get seconds from epoch for the broken-down time representation.
   */
  int64 toEpoch() const;

  /**
   * Return broken-down UTC time representing the current time.
   */
  static Time utc();

  /**
   * Return broken-down UTC time representing the time given as seconds from epoch.
   */
  static Time utc(int64 epoch);

  /**
   * Return broken-down local time representing the current time.
   */
  static Time local();

  /**
   * Return broken-down local time representing the time given as seconds from epoch.
   */
  static Time local(int64 epoch);

  /**
   * Convert to ISO-like date/time string "yyyy-mm-dd hh:mm:ss".
   */
  String toString() const;

};

}
