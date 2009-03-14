/*
 *  LogFile.cpp
 *
 *  Utility for writing a log file
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "base.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

namespace oz
{

  LogFile logFile;

  // first parameter is file name, the other tells us if we want to clear its content if
  // the file already exists
  bool LogFile::init( const char *fileName, bool clearFile, const char *indentStr_ )
  {
    // initalize ident
    tabs = 0;

    indentStr = indentStr_ != null ? indentStr_ : "  ";

    if( fileName && fileName[0] != '\0' ) {
      isStdout = false;
      logFile = fileName;

      // clear the file if necessary
      if( clearFile ) {
        FILE *f = fopen( logFile, "w" );

        if( f != null ) {
          fclose( f );
        }
        else {
          return false;
        }
      }
    }
    else {
      isStdout = true;
    }

    return true;
  }

  void LogFile::printRaw( const char *s, ... ) const
  {
    va_list ap;
    FILE *f;

    f = isStdout ? stdout : fopen( logFile, "a" );

    va_start( ap, s );
    vfprintf( f, s, ap );
    va_end( ap );

    if( !isStdout ) {
      fclose( f );
    }
  }

  void LogFile::print( const char *s, ... ) const
  {
    va_list ap;
    FILE *f;

    f = isStdout ? stdout : fopen( logFile, "a" );

    for( int i = 0; i < tabs; i++ ) {
      fprintf( f, "%s", indentStr.cstr() );
    }
    va_start( ap, s );
    vfprintf( f, s, ap );
    va_end( ap );

    if( !isStdout ) {
      fclose( f );
    }
  }

  void LogFile::println( const char *s, ... ) const
  {
    va_list ap;
    FILE *f;

    f = isStdout ? stdout : fopen( logFile, "a" );

    for( int i = 0; i < tabs; i++ ) {
      fprintf( f, "%s", indentStr.cstr() );
    }
    va_start( ap, s );
    vfprintf( f, s, ap );
    va_end( ap );

    fprintf( f, "\n" );

    if( !isStdout ) {
      fclose( f );
    }
  }

  void LogFile::printlnBT( const char *s, ... ) const
  {
    va_list ap;
    FILE *f;
    time_t ct;                 // current time
    struct tm t;

    ct = time( null );         // get current time
    t = *localtime( &ct );

    f = isStdout ? stdout : fopen( logFile, "a" );

    for( int i = 0; i < tabs; i++ ) {
      fprintf( f, "%s", indentStr.cstr() );
    }
    fprintf( f, "%02d:%02d:%02d ", t.tm_hour, t.tm_min, t.tm_sec );

    va_start( ap, s );
    vfprintf( f, s, ap );
    va_end( ap );

    fprintf( f, "\n" );

    if( !isStdout ) {
      fclose( f );
    }
  }

  void LogFile::printlnET( const char *s, ... ) const
  {
    va_list ap;
    FILE *f;
    time_t ct;
    struct tm t;

    ct = time( null );
    t = *localtime( &ct );

    f = isStdout ? stdout : fopen( logFile, "a" );

    for( int i = 0; i < tabs; i++ ) {
      fprintf( f, "%s", indentStr.cstr() );
    }
    va_start( ap, s );
    vfprintf( f, s, ap );
    va_end( ap );

    fprintf( f, " %02d:%02d:%02d\n", t.tm_hour, t.tm_min, t.tm_sec );

    if( !isStdout ) {
      fclose( f );
    }
  }

  void LogFile::printlnETD( const char *s, ... ) const
  {
    va_list ap;
    FILE *f;
    time_t ct;
    struct tm t;

    ct = time( null );
    // transformation to years, months and days is too complicated, using function localtime
    t = *localtime( &ct );

    f = isStdout ? stdout : fopen( logFile, "a" );

    for( int i = 0; i < tabs; i++ ) {
      fprintf( f, "%s", indentStr.cstr() );
    }
    va_start( ap, s );
    vfprintf( f, s, ap );
    va_end( ap );

    fprintf( f, " %02d.%02d.%04d %02d:%02d:%02d\n", t.tm_mday, t.tm_mon + 1, t.tm_year + 1900,
             t.tm_hour, t.tm_min, t.tm_sec );

    if( !isStdout ) {
      fclose( f );
    }
  }

  void LogFile::println() const
  {
    FILE *f;

    f = isStdout ? stdout : fopen( logFile, "a" );

    fprintf( f, "\n" );

    if( !isStdout ) {
      fclose( f );
    }
  }

  void LogFile::indent()
  {
    tabs++;
  }

  void LogFile::unindent()
  {
    if( tabs ) {
      tabs--;
    }
  }

  void LogFile::clear() const
  {
    if( !isStdout ) {
      FILE *f = fopen( logFile, "w" );
      fclose( f );
    }
  }

}
