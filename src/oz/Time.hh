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

#include "common.hh"

namespace oz
{

/**
 * %Time structure.
 *
 * @ingroup oz
 */
class Time
{
  public:

    long64 epoch;  ///< Seconds since epoch, 1970-01-01 0:00:00.
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
     * Sleep specified number of milliseconds.
     */
    static void sleep( uint milliseconds );

    /**
     * Monotonic clock from an unspecified point in time, with microsecond resolution.
     *
     * This clock wraps around in about 72 min.
     */
    static uint uclock();

    /**
     * Sleep specified number of microseconds.
     *
     * On Windows usleep has millisecond granularity; it this case <tt>microseconds</tt> is rounded
     * to the nearest millisecond value greater than zero.
     */
    static void usleep( uint microseconds );

    /**
     * Get current Unix time (seconds from epoch, 1970-01-01 0:00:00).
     */
    static long64 time();

    /**
     * Return <tt>Time</tt> structure filled with the current UTC time.
     */
    static Time utc();

    /**
     * Fill year, month ... etc. fields from Unix time.
     */
    static Time utc( long64 epoch );

    /**
     * Return <tt>Time</tt> structure filled with the current local time.
     */
    static Time local();

    /**
     * Fill year, month ... etc. fields from Unix time.
     */
    static Time local( long64 epoch );

};

}
