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
 * @file oz/StackTrace.hh
 *
 * StackTrace class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Stack trace entry.
 */
class StackTrace
{
  public:

    /// Maximum number of stack frames.
    static const int MAX_FRAMES = 32;

    int   nFrames;            ///< Number of stack frames.
    void* frames[MAX_FRAMES]; ///< Pointers to stack frames.

  public:

    /**
     * Generates array of frame pointers and return it in `StackTrace` class.
     *
     * @param nSkippedFrames number of stack frames to skip beside the stack frame of this call.
     * Must be >= -1 (if -1 stack frame of this call is also included).
     */
    static StackTrace current( int nSkippedFrames );

    /**
     * Return string table for stack frames.
     *
     * Beginning of the returned table contains pointers to string entries in stack trace, same as
     * with `backtrace_symbols()` call from glibc. The table must be freed by caller with
     * `std::free()`.
     */
    char** symbols() const;

};

}
