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
 * @file oz/Exception.hh
 *
 * Exception class.
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
                        const char* message, ... ) noexcept;

    /**
     * Message string (no file, stack trace etc.).
     */
    virtual const char* what() const noexcept;

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
