/*
 *  Log.cpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
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

Log::Log() : stream( stdout ), tabs( 0 ), indentStr( "  " )
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
    fprintf( f, "%s", indentStr.cstr() );
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
    fprintf( f, "%s", indentStr.cstr() );
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

void Log::printlnBT( const char* s, ... ) const
{
  FILE* f = reinterpret_cast<FILE*>( stream );

  time_t ct = time( null );
  tm t = *localtime( &ct );

  for( int i = 0; i < tabs; ++i ) {
    fprintf( f, "%s", indentStr.cstr() );
  }
  fprintf( f, "%02d:%02d:%02d ", t.tm_hour, t.tm_min, t.tm_sec );

  va_list ap;
  va_start( ap, s );

  vfprintf( f, s, ap );
  fprintf( f, "\n" );

  va_end( ap );

  fflush( f );
}

void Log::printlnET( const char* s, ... ) const
{
  FILE* f = reinterpret_cast<FILE*>( stream );

  time_t ct = time( null );
  tm t = *localtime( &ct );

  for( int i = 0; i < tabs; ++i ) {
    fprintf( f, "%s", indentStr.cstr() );
  }

  va_list ap;
  va_start( ap, s );

  vfprintf( f, s, ap );
  fprintf( f, " %02d:%02d:%02d\n", t.tm_hour, t.tm_min, t.tm_sec );

  va_end( ap );

  fflush( f );
}

void Log::printlnETD( const char* s, ... ) const
{
  FILE* f = reinterpret_cast<FILE*>( stream );

  time_t ct = time( null );
  tm t = *localtime( &ct );

  for( int i = 0; i < tabs; ++i ) {
    fprintf( f, "%s", indentStr.cstr() );
  }

  va_list ap;
  va_start( ap, s );

  vfprintf( f, s, ap );
  fprintf( f, " %d.%d.%04d %02d:%02d:%02d\n",
           t.tm_mday, t.tm_mon + 1, t.tm_year + 1900,
           t.tm_hour, t.tm_min, t.tm_sec );

  va_end( ap );

  fflush( f );
}

void Log::printTrace( const char* frames, int nFrames ) const
{
  FILE* f = reinterpret_cast<FILE*>( stream );

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
           indentStr.cstr(), e.function,
           indentStr.cstr(), e.file, e.line );

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
