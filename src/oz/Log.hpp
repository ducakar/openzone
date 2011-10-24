/*
 *  Log.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file Log.hpp
 */

#include "Exception.hpp"

namespace oz
{

/**
 * %Log writing utility.
 *
 * Logging service, can write to stdout or into a file. After each operation stream is flushed,
 * so no data is lost on a crash.
 */
class Log
{
  private:

    void*  stream;    ///< %File handle, but declared FILE* because we do not want to pollute
                      ///< namespace with \<cstdio\>.
    int    tabs;      ///< Indentation levels.
    String indentStr; ///< Indent string.

  public:

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
    void vprintRaw( const char* s, va_list ap ) const;

    /**
     * Print the raw text.
     */
    void printRaw( const char* s, ... ) const;

    /**
     * Indent and print the text.
     */
    void print( const char* s, ... ) const;

    /**
     * Print the text and terminate the line.
     */
    void printEnd( const char* s, ... ) const;

    /**
     * Terminate the line.
     */
    void printEnd() const;

    /**
     * Indent, print the text and terminate the line.
     */
    void println( const char* s, ... ) const;

    /**
     * Print a blank line.
     */
    void println() const;

    /**
     * As println, but print the current time at the beginning.
     */
    void printlnBT( const char* s, ... ) const;

    /**
     * As println, but print the current time at the end.
     */
    void printlnET( const char* s, ... ) const;

    /**
     * As println, but print the current time and date at the end.
     */
    void printlnETD( const char* s, ... ) const;

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

/// It is very convenient to have a globally defined log instance and also needed by some classes
/// in liboz.
extern Log log;

}
