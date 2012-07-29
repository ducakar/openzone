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

#if defined( __GLIBC__ ) || defined( _LIBCPP_VERSION )
# include <cstdlib>
# include <cstring>
# include <cxxabi.h>
# include <execinfo.h>
#endif

namespace oz
{

#if !defined( __GLIBC__ ) && !defined( _LIBCPP_VERSION )

StackTrace StackTrace::current( int )
{
  return { 0, {} };
}

char** StackTrace::symbols() const
{
  return null;
}

#else

// Size of output buffer where stack trace output string is generated.
static const int TRACE_BUFFER_SIZE = 4096;

// Maximum size for buffer where function names are demangled.
static const int SYMBOL_BUFFER_SIZE = 256;

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

  int i;
  for( i = 0; i < nFrames; ++i ) {
    // File.
    char* file = symbols[i];

    // Mangled function name.
    char* func = strrchr( symbols[i], '(' );
    char* end  = null;

    if( func != null ) {
      *func++ = '\0';

      end = strrchr( func, '+' );

      if( end != null ) {
        *end = '\0';
      }
    }

    size_t fileLen = strlen( file );
    if( out + fileLen + 4 > outEnd ) {
      break;
    }

    memcpy( out, file, fileLen );
    out += fileLen;

    *out++ = ':';
    *out++ = ' ';

    if( func == null || func >= end ) {
      *out++ = '?';
    }
    else {
      // Demangle name.
      char*  demangled;
      int    status = 0;

      demangled = abi::__cxa_demangle( func, null, null, &status );
      func      = demangled == null ? func : demangled;

      size_t funcLen = strlen( func );

      if( funcLen != 0 && out + funcLen + 1 <= outEnd ) {
        memcpy( out, func, funcLen );
        out += funcLen;
      }
      else {
        *out++ = '?';
      }

      free( demangled );
    }

    *out++ = '\0';
  }

  int nWrittenFrames = i;

  size_t headerSize  = size_t( nWrittenFrames ) * sizeof( char* );
  size_t bodySize    = size_t( out - outputBuffer );
  char** niceSymbols = static_cast<char**>( realloc( symbols, headerSize + bodySize ) );

  if( niceSymbols == null ) {
    free( symbols );
    return null;
  }

  memcpy( &niceSymbols[nWrittenFrames], outputBuffer, bodySize );

  char* entry = reinterpret_cast<char*>( &niceSymbols[nWrittenFrames] );
  for( int i = 0; i < nWrittenFrames; ++i ) {
    niceSymbols[i] = entry;
    entry += strlen( entry ) + 1;
  }

  return niceSymbols;
}

#endif

}
