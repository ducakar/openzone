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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file oz/Log.hpp
 */

#pragma once

#include "Exception.hpp"

namespace oz
{

/**
 * %Log writing utility.
 *
 * Logging service, can write to stdout or into a file. After each operation stream is flushed,
 * so no data is lost on a crash.
 *
 * @ingroup oz
 */
class Log
{
  private:

    void*       stream;    ///< %File handle, but declared FILE* because we do not want to pollute
                           ///< namespace with \<cstdio\>.
    const char* indentStr; ///< Indent string.
    int         tabs;      ///< Indentation levels.

  public:

    bool        isVerbose; ///< It has no influence on <tt>Log</tt>, only for convenience.

    /**
     * Initialise log with default parameters to init().
     */
    Log();

    /**
     * Destructor.
     */
    ~Log();

    /**
     * True iff output stream is a file.
     */
    bool isFile() const;

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
     * Print stack trace returned by <code>System::getStackTrace()</code>.
     */
    void printTrace( const char* frames, int nFrames ) const;

    /**
     * Print nicely formatted exception (unindented).
     */
    void printException( const Exception& e ) const;

    /**
     * First parameter is file name (if null or <tt>""</tt>, it writes to stdout),
     * the other tells us if we want to clear its content if the file already exists.
     */
    bool init( const char* fileName = null, bool clearFile = true, const char* indentStr = "  " );

};

/**
 * It is very convenient to have a globally defined log instance and also needed by some classes
 * in liboz.
 *
 * @ingroup oz
 */
extern Log log;

}
