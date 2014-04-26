/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file ozCore/StackTrace.cc
 */

#include "StackTrace.hh"

#if !defined( __GLIBC__ )
# define OZ_DISABLE_STACK_TRACE
#endif

#include "arrays.hh"
#include "Thread.hh"

#include <cstring>

#ifndef OZ_DISABLE_STACK_TRACE
# include <cstdlib>
# include <cxxabi.h>
# include <execinfo.h>
#endif

namespace oz
{

const int StackTrace::MAX_FRAMES;

#ifdef OZ_DISABLE_STACK_TRACE

StackTrace StackTrace::current( int )
{
  const char* name = Thread::name();

  StackTrace st = { {}, 0, {} };

  if( name != nullptr ) {
    strlcpy( st.threadName, name, NAME_LENGTH );
  }
  return st;
}

char** StackTrace::symbols() const
{
  return nullptr;
}

#else

// Size of output buffer where stack trace output string is generated.
static const int TRACE_BUFFER_SIZE = 4096;

StackTrace StackTrace::current( int nSkippedFrames )
{
  hard_assert( nSkippedFrames >= 0 );

  void*       framesBuffer[StackTrace::MAX_FRAMES + 4];
  const char* name            = Thread::name();
  int         nBufferedFrames = backtrace( framesBuffer, MAX_FRAMES + 4 );
  int         nFrames         = min<int>( nBufferedFrames - 1 - nSkippedFrames, MAX_FRAMES );

  StackTrace st = { {}, nFrames, {} };

  if( name != nullptr ) {
    strlcpy( st.threadName, name, NAME_LENGTH );
  }
  aCopy<void*>( framesBuffer + 1 + nSkippedFrames, st.nFrames, st.frames );
  return st;
}

char** StackTrace::symbols() const
{
  char outputBuffer[TRACE_BUFFER_SIZE];

  char** symbols = backtrace_symbols( frames, nFrames );
  if( symbols == nullptr ) {
    return nullptr;
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
    char* end  = nullptr;

    if( func != nullptr ) {
      *func++ = '\0';

      end = strrchr( func, '+' );

      if( end != nullptr ) {
        *end = '\0';
      }
    }

    size_t fileLen = strlen( file );
    if( out + fileLen + 4 > outEnd ) {
      break;
    }

    mCopy( out, file, fileLen );
    out += fileLen;

    *out++ = ':';
    *out++ = ' ';

    if( func == nullptr || func >= end ) {
      *out++ = '?';
    }
    else {
      // Demangle name.
      char* demangled;
      int   status = 0;

      demangled = abi::__cxa_demangle( func, nullptr, nullptr, &status );
      func      = demangled == nullptr ? func : demangled;

      size_t funcLen = strlen( func );

      if( funcLen != 0 && out + funcLen + 1 <= outEnd ) {
        mCopy( out, func, funcLen );
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

  size_t headerSize = size_t( nWrittenFrames ) * sizeof( char* );
  size_t bodySize   = size_t( out - outputBuffer );

  if( headerSize + bodySize == 0 ) {
    free( symbols );
    return nullptr;
  }

  char** niceSymbols = static_cast<char**>( realloc( symbols, headerSize + bodySize ) );

  if( niceSymbols == nullptr ) {
    free( symbols );
    return nullptr;
  }

  mCopy( &niceSymbols[nWrittenFrames], outputBuffer, bodySize );

  char* entry = reinterpret_cast<char*>( &niceSymbols[nWrittenFrames] );
  for( i = 0; i < nWrittenFrames; ++i ) {
    niceSymbols[i] = entry;
    entry += strlen( entry ) + 1;
  }

  return niceSymbols;
}

#endif

}
