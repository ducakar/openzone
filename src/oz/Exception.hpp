/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
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
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file oz/Exception.hpp
 */

#pragma once

#include "String.hpp"

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

    String      message;  ///< Message.

    const char* file;     ///< %File.
    const char* function; ///< Function name.
    int         line;     ///< Source file line.

    int         nFrames;  ///< Number of frames of stack trace.
    char*       frames;   ///< Stack trace buffer.

    /**
     * %Exception constructor.
     *
     * This constructor is usually not used directly, but via <tt>%Exception</tt> macro.
     * It generates SIGTRAP to signal debugger on exception.
     */
    explicit Exception( const String& message, const char* file, int line,
                        const char* function ) throw();

    /**
     * Destructor.
     */
    virtual ~Exception() throw();

    /**
     * Message string (no file, line number etc.).
     */
    virtual const char* what() const throw();

};

/**
 * @def Exception
 * Exception constructor wrapper that provides the current file and line.
 *
 * @ingroup oz
 */
#define Exception( message ) \
  oz::Exception( message, __FILE__, __LINE__, __PRETTY_FUNCTION__ )

}
