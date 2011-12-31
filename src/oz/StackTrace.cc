/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
