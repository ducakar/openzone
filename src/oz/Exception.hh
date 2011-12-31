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
 * @file oz/Exception.hh
 */

#pragma once

#include "StackTrace.hh"

namespace oz
{

/**
 * %Exception class.
 *
 * @ingroup oz
 */
class Exception : public std::exception
{
  public:

    char        message[256]; ///< Message.
    const char* file;         ///< %File.
    const char* function;     ///< Function name.
    int         line;         ///< Source file line.
    StackTrace  stackTrace;   ///< Stack trace.

    /**
     * <tt>Exception</tt> constructor.
     *
     * This constructor is usually not used directly, but via <tt>Exception</tt> macro.
     * It generates SIGTRAP to signal debugger on exception.
     */
    OZ_PRINTF_FORMAT( 5, 6 )
    explicit Exception( const char* file, int line, const char* function,
                        const char* message, ... ) throw();

    /**
     * Message string (no file, line number etc.).
     */
    virtual const char* what() const throw();

};

/**
 * @def Exception
 * <tt>Exception</tt> constructor wrapper that provides the current file, line and function.
 *
 * @ingroup oz
 */
#define Exception( ... ) \
  oz::Exception( __FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__ )

}
