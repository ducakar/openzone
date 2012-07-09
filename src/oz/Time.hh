/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file oz/Time.hh
 *
 * Time class.
 */

#pragma once

#include "String.hh"

namespace oz
{

/**
 * %Time structure.
 *
 * Keep in mind that epoch may differ between platforms (it is 1970-01-01 0:00:00 on POSIX systems
 * and 1601-01-01 0:00:00 on Windows).
 *
 * @ingroup oz
 */
class Time
{
#ifdef _WIN32

  private:

    /// Internal helper struct used for static initialisation of WIN32 performance counter.
    struct PerformanceTimer;

#endif

  public:

    long64 epoch;  ///< Seconds since platform-dependent epoch.
    int    year;   ///< Year, all digits.
    int    month;  ///< Month, from 1 to 12.
    int    day;    ///< Day in month, from 1 to 31.
    int    hour;   ///< Hour.
    int    minute; ///< Minute.
    int    second; ///< Second.

    /**
     * Monotonic clock from an unspecified point in time, with millisecond resolution.
     *
     * This clock wraps around in about 49.7 days.
     */
    static uint clock();

    /**
     * Sleep for given number of milliseconds.
     */
    static void sleep( uint milliseconds );

    /**
     * Monotonic clock from an unspecified point in time, with microsecond resolution.
     *
     * This clock wraps around in about 72 min.
     */
    static uint uclock();

    /**
     * Sleep for given number of microseconds.
     */
    static void usleep( uint microseconds );

    /**
     * Get current time in seconds from platform-dependent epoch.
     */
    static long64 time();

    /**
     * Return <tt>Time</tt> structure filled with the current local time.
     */
    static Time local();

    /**
     * Fill year, month ... etc. fields from time given as seconds from epoch.
     */
    static Time local( long64 epoch );

    /**
     * Get seconds from epoch for the given year, month ... etc. fields in local time zone.
     */
    long64 toEpoch() const;

    /**
     * Convert to ISO date/time string (yyyy-mm-dd hh:mm:ss).
     */
    String toString() const;

};

}
