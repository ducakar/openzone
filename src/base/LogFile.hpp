/*
 *  LogFile.hpp
 *
 *  Utility for writing a log file
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  class LogFile
  {
    private:

      String logFile;                            // name of log file
      String indentStr;                          // indent string
      int    tabs;                               // amount of indent ( one indent is 3 spaces )
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
       * Indents and prints the text.
       * @param s
       */
      void print( const char *s, ... ) const;

      /**
       * Do not indent.
       * @param s
       */
      void printRaw( const char *s, ... ) const;

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
       * Increases ident.
       */
      void indent();

      /**
       * Decreases ident.
       */
      void unindent();

      /**
       * Clears content of a file.
       */
      void clear() const;
  };

  extern LogFile logFile;

}
