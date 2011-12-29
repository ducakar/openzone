/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2011 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file oz/Log.cc
 */

#include "Log.hh"

#include "Time.hh"

#include "windefs.h"

#include <cstdio>
#include <cstdlib>

namespace oz
{

Log log;

Log::Log() : fileStream( null ), tabs( 0 ), isVerbose( false )
{}

Log::~Log()
{
  FILE* file = reinterpret_cast<FILE*>( fileStream );

  if( file != null ) {
    fclose( file );
    file = null;
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

void Log::vprintRaw( const char* s, va_list ap ) const
{
  FILE* file = reinterpret_cast<FILE*>( fileStream );

  va_list ap2;
  va_copy( ap2, ap );

  vprintf( s, ap );

  if( file != null ) {
    vfprintf( file, s, ap2 );

    fflush( file );
  }
}

void Log::printRaw( const char* s, ... ) const
{
  FILE* file = reinterpret_cast<FILE*>( fileStream );

  va_list ap;
  va_start( ap, s );

  vprintf( s, ap );

  va_end( ap );

  if( file != null ) {
    va_start( ap, s );

    vfprintf( file, s, ap );

    va_end( ap );

    fflush( file );
  }
}

void Log::print( const char* s, ... ) const
{
  FILE* file = reinterpret_cast<FILE*>( fileStream );

  for( int i = 0; i < tabs; ++i ) {
    printf( "  " );

    if( file != null ) {
      fprintf( file, "  " );
    }
  }

  va_list ap;
  va_start( ap, s );

  vprintf( s, ap );

  va_end( ap );

  if( file != null ) {
    va_start( ap, s );

    vfprintf( file, s, ap );

    va_end( ap );

    fflush( file );
  }
}

void Log::printEnd( const char* s, ... ) const
{
  FILE* file = reinterpret_cast<FILE*>( fileStream );

  va_list ap;
  va_start( ap, s );

  vprintf( s, ap );
  printf( "\n" );

  va_end( ap );

  if( file != null ) {
    va_start( ap, s );

    vfprintf( file, s, ap );
    fprintf( file, "\n" );

    va_end( ap );

    fflush( file );
  }
}

void Log::printEnd() const
{
  FILE* file = reinterpret_cast<FILE*>( fileStream );

  printf( "\n" );

  if( file != null ) {
    fprintf( file, "\n" );

    fflush( file );
  }
}

void Log::println( const char* s, ... ) const
{
  FILE* file = reinterpret_cast<FILE*>( fileStream );

  for( int i = 0; i < tabs; ++i ) {
    printf( "  " );

    if( file != null ) {
      fprintf( file, "  " );
    }
  }

  va_list ap;
  va_start( ap, s );

  vprintf( s, ap );
  printf( "\n" );

  va_end( ap );

  if( file != null ) {
    va_start( ap, s );

    vfprintf( file, s, ap );
    fprintf( file, "\n" );

    va_end( ap );

    fflush( file );
  }
}

void Log::println() const
{
  FILE* file = reinterpret_cast<FILE*>( fileStream );

  printf( "\n" );

  if( file != null ) {
    fprintf( file, "\n" );

    fflush( file );
  }
}

void Log::printTime() const
{
  FILE* file = reinterpret_cast<FILE*>( fileStream );

  Time time = Time::local();

  printf( "%04d-%02d-%02d %02d:%02d:%02d",
          time.year, time.month, time.day, time.hour, time.minute, time.second );

  if( file != null ) {
    fprintf( file, "%04d-%02d-%02d %02d:%02d:%02d",
             time.year, time.month, time.day, time.hour, time.minute, time.second );

    fflush( file );
  }
}

void Log::printTrace( const StackTrace* st ) const
{
  FILE* file = reinterpret_cast<FILE*>( fileStream );

  if( st->nFrames == 0 ) {
    printf( "    [empty stack trace]\n" );

    if( file != null ) {
      fprintf( file, "    [empty stack trace]\n" );
    }
  }
  else {
    char** entries = st->symbols();

    for( int i = 0; i < st->nFrames; ++i ) {
      printf( "    %s\n", entries[i] );

      if( file != null ) {
        fprintf( file, "    %s\n", entries[i] );
      }
    }

    free( entries );
  }

  if( file != null ) {
    fflush( file );
  }
}

void Log::printException( const std::exception& e ) const
{
  FILE* file = reinterpret_cast<FILE*>( fileStream );

  const Exception* oe = dynamic_cast<const Exception*>( &e );

  if( oe == null ) {
    printf( "\n\nEXCEPTION: %s\n", e.what() );

    if( file != null ) {
      fprintf( file, "\n\nEXCEPTION: %s\n", e.what() );

      fflush( file );
    }
  }
  else {
    const Exception& e = *oe;

    printf( "\n\nEXCEPTION: %s\n  in %s\n  at %s:%d\n  stack trace:\n",
            e.what(), e.function, e.file, e.line );

    if( file != null ) {
      fprintf( file, "\n\nEXCEPTION: %s\n  in %s\n  at %s:%d\n  stack trace:\n",
               e.what(), e.function, e.file, e.line );
    }

    if( e.stackTrace.nFrames == 0 ) {
      printf( "    [empty stack trace]\n" );

      if( file != null ) {
        fprintf( file, "    [empty stack trace]\n" );
      }
    }
    else {
      char** entries = e.stackTrace.symbols();

      for( int i = 0; i < e.stackTrace.nFrames; ++i ) {
        printf( "    %s\n", entries[i] );

        if( file != null ) {
          fprintf( file, "    %s\n", entries[i] );
        }
      }

      free( entries );
    }

    if( file != null ) {
      fflush( file );
    }
  }
}

bool Log::init( const char* fileName, bool doClear )
{
  FILE* file = reinterpret_cast<FILE*>( fileStream );

  tabs = 0;

  if( file != null ) {
    fclose( file );
    fileStream = null;
  }

  if( fileName != null && fileName[0] != '\0' ) {
    fileStream = fopen( fileName, doClear ? "w" : "a" );
  }

  return fileStream != null;
}

}
