/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file oz/Time.hh
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

    int year;   ///< Year, all digits.
    int month;  ///< Month, from 1 to 12.
    int day;    ///< Day in month, from 1 to 31.
    int hour;   ///< Hour.
    int minute; ///< Minute.
    int second; ///< Second.

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
     * Return <tt>Time</tt> structure filled with the current UTC time.
     */
    static Time utc();

    /**
     * Return <tt>Time</tt> structure filled with the current local time.
     */
    static Time local();

};

}
