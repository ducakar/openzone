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
 * @file oz/StackTrace.cc
 */

#include "StackTrace.hh"

#include "arrays.hh"

#ifdef __GLIBC__
# include <execinfo.h>
# include <cstdlib>
# include <cstring>
# include <cxxabi.h>
#endif

namespace oz
{

#ifndef __GLIBC__

StackTrace StackTrace::current( int )
{
  return { 0, {} };
}

char** StackTrace::symbols() const
{
  return null;
}

#else

// Size of internal output buffer where stack trace output string is generated.
static const int TRACE_BUFFER_SIZE = 2048;

// Size of internal buffer where function names are demangled.
static const int STRING_BUFFER_SIZE = 256;

StackTrace StackTrace::current( int nSkippedFrames )
{
  hard_assert( nSkippedFrames >= -1 );

  void* framesBuffer[StackTrace::MAX_FRAMES + 4];
  int nFrames = backtrace( framesBuffer, MAX_FRAMES + 4 );

  StackTrace st;
  st.nFrames = min<int>( nFrames - 1 - nSkippedFrames, int( MAX_FRAMES ) );
  aCopy( st.frames, framesBuffer + 1 + nSkippedFrames, st.nFrames );
  return st;
}

char** StackTrace::symbols() const
{
  char outputBuffer[TRACE_BUFFER_SIZE];

  char** symbols = backtrace_symbols( frames, nFrames );

  if( symbols == null ) {
    return null;
  }

  const char* const outEnd = outputBuffer + TRACE_BUFFER_SIZE;
  char* out = outputBuffer;

  *out = '\0';

  for( int i = 0; i < nFrames; ++i ) {
    // File.
    char* file = symbols[i];

    // Mangled function name.
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

    // Offset.
    char* offset = strchr( func, '+' );

    if( offset == null ) {
      offset = func - 1;
    }

    *offset = '\0';
    ++offset;

    // Address (plus a leading space).
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

    // Demangle name.
    char*  demangled;
    size_t size = STRING_BUFFER_SIZE;
    int    status = 0;

    demangled = abi::__cxa_demangle( func, null, &size, &status );
    func      = demangled != null ? demangled : func;

    size_t fileLen    = strnlen( file, STRING_BUFFER_SIZE );
    size_t funcLen    = strnlen( func, STRING_BUFFER_SIZE );
    size_t offsetLen  = strnlen( offset, STRING_BUFFER_SIZE );
    size_t addressLen = strnlen( address, STRING_BUFFER_SIZE );

    size = fileLen + 2 + addressLen + 1;
    if( funcLen != 0 && offsetLen != 0 ) {
      size += 1 + funcLen + 3 + offsetLen + 1;
    }

    if( out + size > outEnd ) {
      free( demangled );
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

    free( demangled );
  }

  size_t headerSize  = size_t( nFrames ) * sizeof( char* );
  size_t bodySize    = size_t( out - outputBuffer );
  char** niceSymbols = static_cast<char**>( realloc( symbols, headerSize + bodySize ) );

  if( niceSymbols == null ) {
    free( symbols );
    return niceSymbols;
  }

  memcpy( &niceSymbols[nFrames], outputBuffer, bodySize );

  char* entry = reinterpret_cast<char*>( &niceSymbols[nFrames] );
  for( int i = 0; i < nFrames; ++i ) {
    niceSymbols[i] = entry;
    entry += strlen( entry ) + 1;
  }

  return niceSymbols;
}

#endif

}
