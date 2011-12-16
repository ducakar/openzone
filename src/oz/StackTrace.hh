/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2011 Davorin Učakar
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
 * @file oz/StackTrace.hh
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Stack trace entry.
 *
 * @ingroup oz
 */
class StackTrace
{
  public:

    /// Maximum number of stack frames.
    static const int MAX_FRAMES = 16;

    int   nFrames;            ///< Number of stack frames.
    void* frames[MAX_FRAMES]; ///< Pointers to stack frames.

    /**
     * Generates array of frame pointers and return it in StackTrace struct.
     */
    static StackTrace current();

    /**
     * Return string table for stack frames.
     *
     * Beginning of the returned table contains pointers to string entries in stack trace, same as
     * with <code>backtrace_symbols()</code> call from glibc. The table must be freed by caller with
     * <tt>free()</tt>.
     */
    char** symbols() const;

};

}
