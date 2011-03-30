/*
 *  Log.hpp
 *
 *  Utility for writing a log file
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "Exception.hpp"

namespace oz
{

  class Log
  {
    private:

      void*  stream;          // should be declared as FILE*, but we don't want to include <cstdio>
      int    tabs;            // amount of indent (one indent is 3 spaces)
      String indentStr;       // indent string

    public:

      /**
       * Default contructor.
       * Initialises Log with default parameters to init() (look declaration).
       * @return
       */
      Log();

      /**
       * Destructor closes the stream.
       */
      ~Log();

      /**
       * First parameter is file name (if NULL or "", it writes to stdout), the other tells us
       * if we want to clear its content if the file already exists.
       * @param fileName
       * @param clearFile
       * @param indentStr
       * @return
       */
      bool init( const char* fileName = null, bool clearFile = true, const char* indentStr = "  " );

      /**
       * @return true, if writing in a file
       */
      bool isFile() const;

      /**
       * Prints the text from argument list.
       * @param s
       */
      void vprintRaw( const char* s, va_list ap ) const;

      /**
       * Prints the text.
       * @param s
       */
      void printRaw( const char* s, ... ) const;

      /**
       * Indents and prints the text.
       * @param s
       */
      void print( const char* s, ... ) const;

      /**
       * Indents, prints the text and terminates the line.
       * @param s
       */
      void println( const char* s, ... ) const;

      /**
       * Write a blank line.
       */
      void println() const;

      /**
       * Print rest of the line. (Do not indent and put '\n' at the end.)
       * @param s
       */
      void printEnd( const char* s, ... ) const;

      /**
       * Terminate line. (Do not indent and put '\n' at the end.)
       * @param s
       */
      void printEnd() const;

      /**
       * Adds the current time at the beginning of a line.
       * @param s
       */
      void printlnBT( const char* s, ... ) const;

      /**
       * Adds the current time at the end of a line.
       * @param s
       */
      void printlnET( const char* s, ... ) const;

      /**
       * Adds the current time and date at the end.
       * @param s
       */
      void printlnETD( const char* s, ... ) const;

      /**
       * Prints stack trace returned by System::getStackTrace().
       * @param frames pointer to frame names returned by System::getStackTrace()
       * @param nFrames number of frames returned by System::getStackTrace()
       */
      void printTrace( const char* frames, int nFrames ) const;

      /**
       * Prints nicely formatted exception, unindented.
       * @param e
       */
      void printException( const Exception& e ) const;

      /**
       * Sets indent to none.
       */
      void resetIndent();

      /**
       * Increases indent.
       */
      void indent();

      /**
       * Decreases indent.
       */
      void unindent();

  };

  extern Log log;

}
