/*
 *  Log.cpp
 *
 *  Utility for writing a log file
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "base.h"

#include <cstdarg>
#include <cstdio>
#include <ctime>

namespace oz
{

  Log log;

  // first parameter is file name, the other tells us if we want to clear its content if
  // the file already exists
  bool Log::init( const char *fileName, bool clearFile, const char *indentStr_ )
  {
    // initialize indent
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

  bool Log::isFile() const
  {
    return !isStdout;
  }

  void Log::printEnd( const char *s, ... ) const
  {
    va_list ap;
    FILE *f;

    f = isStdout ? stdout : fopen( logFile, "a" );

    va_start( ap, s );
    vfprintf( f, s, ap );
    fprintf( f, "\n" );
    va_end( ap );

    if( !isStdout ) {
      fclose( f );
    }
  }

  void Log::print( const char *s, ... ) const
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

  void Log::println( const char *s, ... ) const
  {
    va_list ap;
    FILE *f;

    f = isStdout ? stdout : fopen( logFile, "a" );

    for( int i = 0; i < tabs; i++ ) {
      fprintf( f, "%s", indentStr.cstr() );
    }
    va_start( ap, s );
    vfprintf( f, s, ap );
    fprintf( f, "\n" );
    va_end( ap );

    if( !isStdout ) {
      fclose( f );
    }
  }

  void Log::printlnBT( const char *s, ... ) const
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
    fprintf( f, "\n" );
    va_end( ap );

    if( !isStdout ) {
      fclose( f );
    }
  }

  void Log::printlnET( const char *s, ... ) const
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
    fprintf( f, " %02d:%02d:%02d\n", t.tm_hour, t.tm_min, t.tm_sec );
    va_end( ap );

    if( !isStdout ) {
      fclose( f );
    }
  }

  void Log::printlnETD( const char *s, ... ) const
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
    fprintf( f, " %02d.%02d.%04d %02d:%02d:%02d\n", t.tm_mday, t.tm_mon + 1, t.tm_year + 1900,
             t.tm_hour, t.tm_min, t.tm_sec );
    va_end( ap );

    if( !isStdout ) {
      fclose( f );
    }
  }

  void Log::println() const
  {
    FILE *f;

    f = isStdout ? stdout : fopen( logFile, "a" );

    fprintf( f, "\n" );

    if( !isStdout ) {
      fclose( f );
    }
  }

  void Log::resetIndent()
  {
    tabs = 0;
  }

  void Log::indent()
  {
    tabs++;
  }

  void Log::unindent()
  {
    if( tabs ) {
      tabs--;
    }
  }

  void Log::clear() const
  {
    if( !isStdout ) {
      FILE *f = fopen( logFile, "w" );
      fclose( f );
    }
  }

}
