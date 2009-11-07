/*
 *  Log.h
 *
 *  Utility for writing a log file
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

namespace oz
{

  class Log
  {
    private:

      String logFile;                            // name of log file
      String indentStr;                          // indent string
      int    tabs;                               // amount of indent (one indent is 3 spaces)
      bool   isStdout;

    public:
      /**
       * First parameter is file name (if NULL or "", it writes to stdout), the other tells us
       * if we want to clear its content if the file already exists.
       * @param fileName
       * @param clearFile
       * @param indentStr
       * @return
       */
      bool init( const char *fileName, bool clearFile = true, const char *indentStr = "  " );

      /**
       * @return true, if writing in a file
       */
      bool isFile() const;

      /**
       * Indents and prints the text.
       * @param s
       */
      void print( const char *s, ... ) const;

      /**
       * Print rest of the line. (Do not indent and put '\n' at the end.)
       * @param s
       */
      void printEnd( const char *s, ... ) const;

      /**
       * Indents, prints the text and terminates the line.
       * @param s
       */
      void println( const char *s, ... ) const;

      /**
       * Adds the current time at the beginning of a line.
       * @param s
       */
      void printlnBT( const char *s, ... ) const;

      /**
       * Adds the current time at the end of a line.
       * @param s
       */
      void printlnET( const char *s, ... ) const;

      /**
       * Adds the current time and date at the end.
       * @param s
       */
      void printlnETD( const char *s, ... ) const;

      /**
       * Write a blank line.
       */
      void println() const;

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

      /**
       * Clears content of a file.
       */
      void clear() const;
  };

  extern Log log;

}
