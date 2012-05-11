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

#include "StackTrace.hh"

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
  public:

    static bool showVerbose; ///< Do not suppress verbose messages on stdout. Off by default.
    static bool verboseMode; ///< While <tt>verboseMode</tt> is on log output is only written to
                             ///< log file unless <tt>isVerbose</tt> is also on. Off by default.

    /**
     * Singleton.
     */
    Log() = delete;

    /**
     * Return log file path or an empty string if log is printed to stdout only.
     */
    static const char* logFile();

    /**
     * Set indent to zero.
     */
    static void resetIndent();

    /**
     * Increase indent.
     */
    static void indent();

    /**
     * Decrease indent.
     */
    static void unindent();

    /**
     * Print the text from argument list.
     */
    OZ_PRINTF_FORMAT( 1, 0 )
    static void vprintRaw( const char* s, va_list ap );

    /**
     * Print the raw text (without indentation on newline).
     */
    OZ_PRINTF_FORMAT( 1, 2 )
    static void printRaw( const char* s, ... );

    /**
     * Indent and print the text.
     */
    OZ_PRINTF_FORMAT( 1, 2 )
    static void print( const char* s, ... );

    /**
     * Print the text and terminate the line.
     */
    OZ_PRINTF_FORMAT( 1, 2 )
    static void printEnd( const char* s, ... );

    /**
     * Terminate the line.
     */
    static void printEnd();

    /**
     * Indent, print the text and terminate the line.
     */
    OZ_PRINTF_FORMAT( 1, 2 )
    static void println( const char* s, ... );

    /**
     * Print a blank line.
     */
    static void println();

    /**
     * Print current date and time, without indentation or newline.
     */
    static void printTime();

    /**
     * Print stack trace.
     */
    static void printTrace( const StackTrace* st );

    /**
     * Print nicely formatted exception (unindented).
     */
    static void printException( const std::exception* e );

    /**
     * Print 'Caught signal ...' and signal description.
     *
     * This method is only required by signal handler since it has issues with functions that take
     * variable arguments.
     */
    static void printSignal( int sigNum );

    /**
     * Print notice to attach debugger.
     *
     * This method is only required by signal handler since it has issues with functions that take
     * variable arguments.
     */
    static void printHalt();

    /**
     * First parameter is file path (if <tt>null</tt> or "", it only writes to terminal), the other
     * tells us if we want to clear its content if the file already exists.
     */
    static bool init( const char* filePath = null, bool clearFile = true );

    /**
     * Close log file.
     */
    static void free();

};

}
