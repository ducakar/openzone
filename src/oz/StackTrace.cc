/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
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
 * @file oz/StackTrace.cc
 */

#include "StackTrace.hh"

#ifndef _WIN32
# include <cstdlib>
# include <cstring>
# include <cxxabi.h>
# include <execinfo.h>
#endif

namespace oz
{

#ifdef _WIN32

StackTrace StackTrace::current()
{
  StackTrace st;
  st.nFrames = 0;
  return st;
}

char** StackTrace::symbols() const
{
  return null;
}

#else

static const int TRACE_BUFFER_SIZE  = 4096;
static const int STRING_BUFFER_SIZE = 1024;
static OZ_THREAD_LOCAL char output[TRACE_BUFFER_SIZE];

StackTrace StackTrace::current()
{
  StackTrace st;
  st.nFrames = backtrace( st.frames, MAX_FRAMES );
  return st;
}

char** StackTrace::symbols() const
{
  char** symbols = backtrace_symbols( frames, nFrames );

  if( symbols == null ) {
    return null;
  }

  const char* const outEnd = output + TRACE_BUFFER_SIZE;
  char* out = output;

  *out = '\0';

  for( int i = 0; i < nFrames; ++i ) {
    // file
    char* file = symbols[i];

    // mangled function name
    char* func = strrchr( symbols[i], '(' );

    if( func == null ) {
      size_t size = strlen( file ) + 1;

      if( out + size > outEnd ) {
        break;
      }

      memcpy( out, file, size );
      out[size - 1] = '\0';
      out += size;

      continue;
    }

    *func = '\0';
    ++func;

    // offset
    char* offset = strchr( func, '+' );

    if( offset == null ) {
      offset = func - 1;
    }

    *offset = '\0';
    ++offset;

    // address (plus a leading space)
    char* address = strchr( offset, ')' );

    if( address == null ) {
      --func;
      *func = '(';

      --offset;
      *offset = '+';

      size_t size = strlen( file ) + 1;

      if( out + size > outEnd ) {
        break;
      }

      memcpy( out, file, size );
      out[size - 1] = '\0';
      out += size;

      continue;
    }

    *address = '\0';
    ++address;

    // demangle name
    char*  demangleBuf = reinterpret_cast<char*>( malloc( STRING_BUFFER_SIZE ) );
    char*  demangleOut;
    size_t size = STRING_BUFFER_SIZE;
    int    status = 0;

    demangleOut = abi::__cxa_demangle( func, demangleBuf, &size, &status );
    demangleBuf = demangleOut != null ? demangleOut : demangleBuf;
    func        = status == 0 ? demangleOut : func;

    size_t fileLen    = strnlen( file, STRING_BUFFER_SIZE );
    size_t funcLen    = strnlen( func, STRING_BUFFER_SIZE );
    size_t offsetLen  = strnlen( offset, STRING_BUFFER_SIZE );
    size_t addressLen = strnlen( address, STRING_BUFFER_SIZE );

    size = fileLen + 2 + addressLen + 1;
    if( funcLen != 0 && offsetLen != 0 ) {
      size += 1 + funcLen + 3 + offsetLen + 1;
    }

    if( out + size > outEnd ) {
      free( demangleBuf );
      break;
    }

    memcpy( out, file, fileLen );
    out += fileLen;

    *out = '(';
    ++out;

    if( funcLen != 0 && offsetLen != 0 ) {
      *out = ' ';
      ++out;

      memcpy( out, func, funcLen );
      out += funcLen;

      *out = ' ';
      ++out;
      *out = '+';
      ++out;
      *out = ' ';
      ++out;

      memcpy( out, offset, offsetLen );
      out += offsetLen;

      *out = ' ';
      ++out;
    }

    *out = ')';
    ++out;

    memcpy( out, address, addressLen );
    out += addressLen;

    *out = '\0';
    ++out;

    free( demangleBuf );
  }

  size_t headerSize  = size_t( nFrames ) * sizeof( char* );
  size_t bodySize    = size_t( out - output );
  char** niceSymbols = reinterpret_cast<char**>( realloc( symbols, headerSize + bodySize ) );

  memcpy( &niceSymbols[nFrames], output, bodySize );

  char* entry = reinterpret_cast<char*>( &niceSymbols[nFrames] );
  for( int i = 0; i < nFrames; ++i ) {
    niceSymbols[i] = entry;
    entry += strlen( entry ) + 1;
  }

  return niceSymbols;
}

#endif

}
