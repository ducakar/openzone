/*
 *  Exception.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file oz/Exception.hpp
 */

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
# define Exception( message ) \
  oz::Exception( message, __FILE__, __LINE__, __PRETTY_FUNCTION__ )

}
