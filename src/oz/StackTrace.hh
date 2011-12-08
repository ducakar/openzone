/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
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
