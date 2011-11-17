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
 * @file oz/Log.cpp
 */

#include "Log.hpp"

#include <cstdio>
#include <cstring>
#include <ctime>

namespace oz
{

Log log;

Log::Log() : stream( stdout ), indentStr( "  " ), tabs( 0 ), isVerbose( false )
{}

Log::~Log()
{
  FILE* f = reinterpret_cast<FILE*>( stream );

  if( f != stdout ) {
    fclose( f );
  }
}

bool Log::isFile() const
{
  return stream != stdout;
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
  FILE* f = reinterpret_cast<FILE*>( stream );

  vfprintf( f, s, ap );

  fflush( f );
}

void Log::printRaw( const char* s, ... ) const
{
  FILE* f = reinterpret_cast<FILE*>( stream );

  va_list ap;
  va_start( ap, s );

  vfprintf( f, s, ap );

  va_end( ap );

  fflush( f );
}

void Log::print( const char* s, ... ) const
{
  FILE* f = reinterpret_cast<FILE*>( stream );

  for( int i = 0; i < tabs; ++i ) {
    fprintf( f, "%s", indentStr );
  }

  va_list ap;
  va_start( ap, s );

  vfprintf( f, s, ap );

  va_end( ap );

  fflush( f );
}

void Log::printEnd( const char* s, ... ) const
{
  FILE* f = reinterpret_cast<FILE*>( stream );

  va_list ap;
  va_start( ap, s );

  vfprintf( f, s, ap );
  fprintf( f, "\n" );

  va_end( ap );

  fflush( f );
}

void Log::printEnd() const
{
  FILE* f = reinterpret_cast<FILE*>( stream );

  fprintf( f, "\n" );

  fflush( f );
}

void Log::println( const char* s, ... ) const
{
  FILE* f = reinterpret_cast<FILE*>( stream );

  for( int i = 0; i < tabs; ++i ) {
    fprintf( f, "%s", indentStr );
  }

  va_list ap;
  va_start( ap, s );

  vfprintf( f, s, ap );
  fprintf( f, "\n" );

  va_end( ap );

  fflush( f );
}

void Log::println() const
{
  FILE* f = reinterpret_cast<FILE*>( stream );

  fprintf( f, "\n" );

  fflush( f );
}

void Log::printTime() const
{
  FILE* f = reinterpret_cast<FILE*>( stream );

  time_t ct = time( null );
  tm t = *localtime( &ct );

  fprintf( f, "%04d-%02d-%02d %02d:%02d:%02d",
           t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
           t.tm_hour, t.tm_min, t.tm_sec );

  fflush( f );
}

void Log::printTrace( const char* frames, int nFrames ) const
{
  FILE* f = reinterpret_cast<FILE*>( stream );

  if( nFrames == 0 ) {
    for( int i = 0; i < tabs; ++i ) {
      fprintf( f, "%s", indentStr );
    }
    fprintf( f, "[empty stack trace]\n" );
  }
  else {
    const char* entry = frames;

    for( int i = 0; i < nFrames; ++i ) {
      for( int j = 0; j < tabs; ++j ) {
        fprintf( f, "%s", indentStr );
      }
      fprintf( f, "%s\n", entry );
      entry += strlen( entry ) + 1;
    }
  }

  fflush( f );
}

void Log::printException( const Exception& e ) const
{
  FILE* f = reinterpret_cast<FILE*>( stream );

  fprintf( f,
           "\n"
           "EXCEPTION: %s\n"
           "%sin %s\n"
           "%sat %s:%d\n",
           e.what(),
           indentStr, e.function,
           indentStr, e.file, e.line );

  fprintf( f, "%sstack trace:\n", indentStr );

  if( e.nFrames == 0 ) {
    fprintf( f, "%s%s[empty stack trace]\n", indentStr, indentStr );
  }
  else {
    const char* entry = e.frames;

    for( int i = 0; i < e.nFrames; ++i ) {
      fprintf( f, "%s%s%s\n", indentStr, indentStr, entry );
      entry += strlen( entry ) + 1;
    }
  }

  fflush( f );
}

bool Log::init( const char* fileName, bool doClear, const char* indentStr_ )
{
  FILE* f = reinterpret_cast<FILE*>( stream );

  tabs = 0;
  indentStr = indentStr_ != null ? indentStr_ : "  ";

  if( f != stdout ) {
    fclose( f );
  }

  if( fileName != null && fileName[0] != '\0' ) {
    FILE* f = fopen( fileName, doClear ? "w" : "a" );

    if( f == null ) {
      stream = stdout;
      return false;
    }
    stream = f;
  }
  else {
    stream = stdout;
  }
  return true;
}

}
