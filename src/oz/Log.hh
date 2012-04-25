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

    void* fileStream;    ///< %Log file stream, but declared void* because we don't want to
                         ///< pollute namespace with <tt>\<cstdio\></tt>.
    char  filePath[256]; ///< Log file name.
    int   tabs;          ///< Indentation level (one level is represented with two spaces).

  public:

    bool  isVerbose;     ///< Do not suppress verbose messages on stdout. Off by default.
    bool  verboseMode;   ///< While <tt>verboseMode</tt> is on log output is only written to log
                         ///< file unless <tt>isVerbose</tt> is also on. Off by default.

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
     * Return log file path or an empty string if log is printed to stdout only.
     */
    const char* logFile() const;

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
    bool init( const char* filePath = null, bool clearFile = true );

};

/**
 * It is very convenient to have a globally defined log instance and also needed by some classes
 * in liboz.
 *
 * @ingroup oz
 */
extern Log log;

}
