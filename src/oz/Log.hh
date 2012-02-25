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
 * @file oz/Log.hh
 *
 * Log class.
 */

#pragma once

#include "Exception.hh"

namespace oz
{

/**
 * %Log writing utility.
 *
 * Logging service, can write to terminal and into a file. After each operation stream is flushed,
 * so no data is lost on a crash.
 *
 * Two spaces are used for indentation.
 *
 * @ingroup oz
 */
class Log
{
  private:

    void* fileStream;  ///< %Log file stream, but declared void* because we don't want to
                       ///< pollute namespace with <tt>\<cstdio\></tt>.
    int   tabs;        ///< Indentation level.

  public:

    bool  isVerbose;   ///< Do not suppress messages on stdout in verbose mode. Off by default.
    bool  verboseMode; ///< While <tt>verboseMode</tt> is on log output is only written to log file
                       ///< unless <tt>isVerbose</tt> is also on. Off by default.

    /**
     * Initialise log with default parameters to <tt>init()</tt>.
     */
    Log();

    /**
     * Destructor.
     */
    ~Log();

    /**
     * No copying.
     */
    Log( const Log& ) = delete;

    /**
     * No copying.
     */
    Log& operator = ( const Log& ) = delete;

    /**
     * Set indent to zero.
     */
    void resetIndent();

    /**
     * Increase indent.
     */
    void indent();

    /**
     * Decrease indent.
     */
    void unindent();

    /**
     * Print the text from argument list.
     */
    OZ_PRINTF_FORMAT( 2, 0 )
    void vprintRaw( const char* s, va_list ap ) const;

    /**
     * Print the raw text (without indentation on newline).
     */
    OZ_PRINTF_FORMAT( 2, 3 )
    void printRaw( const char* s, ... ) const;

    /**
     * Indent and print the text.
     */
    OZ_PRINTF_FORMAT( 2, 3 )
    void print( const char* s, ... ) const;

    /**
     * Print the text and terminate the line.
     */
    OZ_PRINTF_FORMAT( 2, 3 )
    void printEnd( const char* s, ... ) const;

    /**
     * Terminate the line.
     */
    void printEnd() const;

    /**
     * Indent, print the text and terminate the line.
     */
    OZ_PRINTF_FORMAT( 2, 3 )
    void println( const char* s, ... ) const;

    /**
     * Print a blank line.
     */
    void println() const;

    /**
     * Print current date and time, without indentation or newline.
     */
    void printTime() const;

    /**
     * Print stack trace.
     */
    void printTrace( const StackTrace* st ) const;

    /**
     * Print nicely formatted exception (unindented).
     */
    void printException( const std::exception* e ) const;

    /**
     * First parameter is file path (if <tt>null</tt> or "", it only writes to terminal), the other
     * tells us if we want to clear its content if the file already exists.
     */
    bool init( const char* fileName = null, bool clearFile = true );

};

/**
 * It is very convenient to have a globally defined log instance and also needed by some classes
 * in liboz.
 *
 * @ingroup oz
 */
extern Log log;

}
