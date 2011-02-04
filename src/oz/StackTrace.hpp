/*
 *  StackTrace.hpp
 *
 *  Class for generating stack trace for the current function call.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "common.hpp"

namespace oz
{

  class StackTrace
  {
    private:

      static const int TRACE_SIZE         = 16;
      static const int TRACE_BUFFER_SIZE  = 4096;
      static const int STRING_BUFFER_SIZE = 1024;

      static const char* const SIGNALS[][2];

      static void* framePtrs[StackTrace::TRACE_SIZE + 1];
      static char  output[StackTrace::TRACE_BUFFER_SIZE];

      static void signalHandler( int signum );

    public:

      /**
       * Set signal handlers so that signal information and stack trace is printed when a signal
       * is raised.
       */
      static void init();

      /**
       * Generates and demangles stack trace.
       * On success it allocates a buffer via malloc where it stores null-byte-separated string
       * of frame names. The caller should free buffer.
       * On failure 0 is returned and bufferPtr is unchanged.
       * @param bufferPtr where to save pointer to buffer
       * @return number of frames
       */
      static int get( char** bufferPtr );

  };

}
