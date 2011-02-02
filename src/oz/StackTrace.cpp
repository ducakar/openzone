/*
 *  StackTrace.hpp
 *
 *  Class for generating stack trace for the current function call.
 *  This class only has effect if OZ_ENABLE_STACKTRACE is enabled.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "StackTrace.hpp"

#include "common.hpp"

#ifdef OZ_ENABLE_STACKTRACE
# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <cxxabi.h>
# include <execinfo.h>
#endif

namespace oz
{

  const int TRACE_SIZE          = 16;
  const int TRACE_BUFFER_SIZE   = 4096;
  const int STRING_BUFFER_SIZE  = 1024;

#ifndef OZ_ENABLE_STACKTRACE

  int StackTrace::get( char** )
  {
    return 0;
  }

#else

  static void* framePtrs[TRACE_SIZE + 1];
  static char  output[TRACE_BUFFER_SIZE];

  int StackTrace::get( char** bufferPtr )
  {
    int    nFrames = backtrace( framePtrs, TRACE_SIZE + 1 ) - 1;
    char** frames  = backtrace_symbols( framePtrs + 1, nFrames );

    if( frames == null ) {
      return 0;
    }

    const char* outEnd = output + TRACE_BUFFER_SIZE;
    char* out = output;

    *out = '\0';

    for( int i = 0; i < nFrames; ++i ) {
      // file
      char* file = frames[i];

      // mangled function name
      char* func = strrchr( frames[i], '(' );

      if( func == null ) {
        free( frames );
        return 0;
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
        free( frames );
        return 0;
      }

      *address = '\0';
      ++address;

      // demangle name
      char*  demangleBuf = reinterpret_cast<char*>( malloc( STRING_BUFFER_SIZE ) );
      char*  demangleOut;
      char*  demangled;
      size_t size = STRING_BUFFER_SIZE;
      int    status = 0;

      demangleOut = abi::__cxa_demangle( func, demangleBuf, &size, &status );
      demangleBuf = demangleOut != null ? demangleOut : demangleBuf;
      demangled   = status == 0 ? demangleOut : func;

      size_t fileLen      = strnlen( file, size );
      size_t demangledLen = strnlen( demangled, size );
      size_t offsetLen    = strnlen( offset, size );
      size_t addressLen   = strnlen( address, size );

      size = fileLen + 2 + addressLen;
      size = demangledLen != 0 && offsetLen != 0 ? size + demangledLen + 3 + offsetLen : size;

      if( out + size >= outEnd ) {
        free( demangleBuf );
        break;
      }

      memcpy( out, file, fileLen );
      out += fileLen;

      *out = '(';
      ++out;

      if( demangledLen != 0 && offsetLen != 0 ) {
        *out = ' ';
        ++out;

        memcpy( out, demangled, demangledLen );
        out += demangledLen;

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

    free( frames );

    *bufferPtr = reinterpret_cast<char*>( malloc( out - output ) );
    memcpy( *bufferPtr, output, out - output );
    return nFrames;
  }

#endif

}
