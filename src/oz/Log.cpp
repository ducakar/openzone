/*
 *  Log.cpp
 *
 *  Utility for writing a log file
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "Log.hpp"

#include <cstdio>
#include <cstring>
#include <ctime>

namespace oz
{

  Log log;

  Log::Log()
  {
    init();
  }

  // first parameter is file name, the other tells us if we want to clear its content if
  // the file already exists
  bool Log::init( const char* fileName, bool clearFile, const char* indentStr_ )
  {
    // initialise indent
    tabs = 0;

    indentStr = indentStr_ != null ? indentStr_ : "  ";

    if( fileName != null && fileName[0] != '\0' ) {
      isStdout = false;
      logFile = fileName;

      // clear the file if necessary
      if( clearFile ) {
        FILE* f = fopen( logFile, "w" );

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

  void Log::printRaw( const char* s, ... ) const
  {
    va_list ap;
    FILE* f;

    f = isStdout ? stdout : fopen( logFile, "a" );

    va_start( ap, s );
    vfprintf( f, s, ap );
    va_end( ap );

    if( !isStdout ) {
      fclose( f );
    }
  }

  void Log::print( const char* s, ... ) const
  {
    va_list ap;
    FILE* f;

    f = isStdout ? stdout : fopen( logFile, "a" );

    for( int i = 0; i < tabs; ++i ) {
      fprintf( f, "%s", indentStr.cstr() );
    }
    va_start( ap, s );
    vfprintf( f, s, ap );
    va_end( ap );

    if( !isStdout ) {
      fclose( f );
    }
  }

  void Log::println( const char* s, ... ) const
  {
    va_list ap;
    FILE* f;

    f = isStdout ? stdout : fopen( logFile, "a" );

    for( int i = 0; i < tabs; ++i ) {
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

  void Log::println() const
  {
    FILE* f;

    f = isStdout ? stdout : fopen( logFile, "a" );

    fprintf( f, "\n" );

    if( !isStdout ) {
      fclose( f );
    }
  }

  void Log::printEnd( const char* s, ... ) const
  {
    va_list ap;
    FILE* f;

    f = isStdout ? stdout : fopen( logFile, "a" );

    va_start( ap, s );
    vfprintf( f, s, ap );
    fprintf( f, "\n" );
    va_end( ap );

    if( !isStdout ) {
      fclose( f );
    }
  }

  void Log::printEnd() const
  {
    FILE* f;

    f = isStdout ? stdout : fopen( logFile, "a" );

    fprintf( f, "\n" );

    if( !isStdout ) {
      fclose( f );
    }
  }

  void Log::printlnBT( const char* s, ... ) const
  {
    va_list ap;
    FILE* f;
    time_t ct;                 // current time
    tm t;

    ct = time( null );         // get current time
    t = *localtime( &ct );

    f = isStdout ? stdout : fopen( logFile, "a" );

    for( int i = 0; i < tabs; ++i ) {
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

  void Log::printlnET( const char* s, ... ) const
  {
    va_list ap;
    FILE* f;
    time_t ct;
    tm t;

    ct = time( null );
    t = *localtime( &ct );

    f = isStdout ? stdout : fopen( logFile, "a" );

    for( int i = 0; i < tabs; ++i ) {
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

  void Log::printlnETD( const char* s, ... ) const
  {
    va_list ap;
    FILE* f;
    time_t ct;
    tm t;

    ct = time( null );
    // transformation to years, months and days is too complicated, using function localtime
    t = *localtime( &ct );

    f = isStdout ? stdout : fopen( logFile, "a" );

    for( int i = 0; i < tabs; ++i ) {
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

#ifndef OZ_ENABLE_STACKTRACE

  void Log::printTrace( const char*, int )
  {}

#else

  void Log::printTrace( const char* frames, int nFrames )
  {
    FILE* f;

    f = isStdout ? stdout : fopen( logFile, "a" );

    if( nFrames == 0 ) {
      for( int i = 0; i < tabs; ++i ) {
        fprintf( f, "%s", indentStr.cstr() );
      }
      fprintf( f, "[empty stack trace]\n" );
    }
    else {
      const char* entry = frames;

      for( int i = 0; i < nFrames; ++i ) {
        for( int j = 0; j < tabs; ++j ) {
          fprintf( f, "%s", indentStr.cstr() );
        }
        fprintf( f, "%s\n", entry );
        entry += strlen( entry ) + 1;
      }
    }

    if( !isStdout ) {
      fclose( f );
    }
  }

#endif

  void Log::printException( const Exception& e )
  {
    FILE* f;

    f = isStdout ? stdout : fopen( logFile, "a" );

    fprintf( f,
             "\n"
             "EXCEPTION: %s\n"
             "%sin %s\n"
             "%sat %s:%d\n",
             e.what(),
             indentStr.cstr(), e.function,
             indentStr.cstr(), e.file, e.line );

#ifdef OZ_ENABLE_STACKTRACE
    fprintf( f, "%sstack trace:\n", indentStr.cstr() );

    if( e.nFrames == 0 ) {
      fprintf( f, "%s%s[empty stack trace]\n", indentStr.cstr(), indentStr.cstr() );
    }
    else {
      const char* entry = e.frames;

      for( int i = 0; i < e.nFrames; ++i ) {
        fprintf( f, "%s%s%s\n", indentStr.cstr(), indentStr.cstr(), entry );
        entry += strlen( entry ) + 1;
      }
    }
#endif

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
    ++tabs;
  }

  void Log::unindent()
  {
    if( tabs > 0 ) {
      --tabs;
    }
  }

  void Log::clear() const
  {
    if( !isStdout ) {
      FILE* f = fopen( logFile, "w" );
      fclose( f );
    }
  }

}
