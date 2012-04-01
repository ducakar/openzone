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

Log::Log() :
  fileStream( null ), tabs( 0 ), isVerbose( false ), verboseMode( false )
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

  if( !verboseMode || isVerbose ) {
    vprintf( s, ap );
  }
  if( file != null ) {
    vfprintf( file, s, ap2 );

    fflush( file );
  }
}

void Log::printRaw( const char* s, ... ) const
{
  FILE* file = reinterpret_cast<FILE*>( fileStream );

  va_list ap;

  if( !verboseMode || isVerbose ) {
    va_start( ap, s );

    vprintf( s, ap );

    va_end( ap );
  }
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

  va_list ap;

  if( !verboseMode || isVerbose ) {
    for( int i = 0; i < tabs; ++i ) {
      printf( "  " );
    }

    va_start( ap, s );

    vprintf( s, ap );

    va_end( ap );
  }
  if( file != null ) {
    for( int i = 0; i < tabs; ++i ) {
      fprintf( file, "  " );
    }

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

  if( !verboseMode || isVerbose ) {
    va_start( ap, s );

    vprintf( s, ap );
    printf( "\n" );

    va_end( ap );
  }
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

  if( !verboseMode || isVerbose ) {
    printf( "\n" );
  }
  if( file != null ) {
    fprintf( file, "\n" );

    fflush( file );
  }
}

void Log::println( const char* s, ... ) const
{
  FILE* file = reinterpret_cast<FILE*>( fileStream );

  va_list ap;

  if( !verboseMode || isVerbose ) {
    for( int i = 0; i < tabs; ++i ) {
      printf( "  " );
    }

    va_start( ap, s );

    vprintf( s, ap );
    printf( "\n" );

    va_end( ap );
  }
  if( file != null ) {
    for( int i = 0; i < tabs; ++i ) {
      fprintf( file, "  " );
    }

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

  if( !verboseMode || isVerbose ) {
    printf( "\n" );
  }
  if( file != null ) {
    fprintf( file, "\n" );

    fflush( file );
  }
}

void Log::printTime() const
{
  FILE* file = reinterpret_cast<FILE*>( fileStream );

  Time time = Time::local();

  if( !verboseMode || isVerbose ) {
    printf( "%04d-%02d-%02d %02d:%02d:%02d",
            time.year, time.month, time.day, time.hour, time.minute, time.second );
  }
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
    if( !verboseMode || isVerbose ) {
      printf( "    [empty stack trace]\n" );
    }
    if( file != null ) {
      fprintf( file, "    [empty stack trace]\n" );
    }
  }
  else {
    char** entries = st->symbols();

    for( int i = 0; i < st->nFrames; ++i ) {
      if( !verboseMode || isVerbose ) {
        printf( "    %s\n", entries[i] );
      }
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

void Log::printException( const std::exception* e ) const
{
  FILE* file = reinterpret_cast<FILE*>( fileStream );

  const Exception* oe = dynamic_cast<const Exception*>( e );

  if( oe == null ) {
    if( !verboseMode || isVerbose ) {
      printf( "\n\nEXCEPTION: %s\n", e->what() );
    }
    if( file != null ) {
      fprintf( file, "\n\nEXCEPTION: %s\n", e->what() );

      fflush( file );
    }
  }
  else {
    if( !verboseMode || isVerbose ) {
      printf( "\n\nEXCEPTION: %s\n  in %s\n  at %s:%d\n  stack trace:\n",
              oe->message, oe->function, oe->file, oe->line );
    }
    if( file != null ) {
      fprintf( file, "\n\nEXCEPTION: %s\n  in %s\n  at %s:%d\n  stack trace:\n",
               oe->message, oe->function, oe->file, oe->line );
    }

    printTrace( &oe->stackTrace );

    if( !verboseMode || isVerbose ) {
      printf( "\n" );
    }
    if( file != null ) {
      fprintf( file, "\n" );
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
