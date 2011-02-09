/*
 *  System.hpp
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

  class System
  {
    private:

      static const int TRACE_SIZE         = 16;
      static const int TRACE_BUFFER_SIZE  = 4096;
      static const int STRING_BUFFER_SIZE = 1024;

      static const char* const SIGNALS[][2];

      static thread_local void* framePtrs[TRACE_SIZE + 1];
      static thread_local char  output[TRACE_BUFFER_SIZE];

      static void signalHandler( int signum );

    public:

      explicit System() = delete;

      /**
       * Set signal handlers to catch critical signals, print information, stack trace and wait
       * for a debugger. SIGINT, SIGQUIT, SIGILL, SIGABRT, SIGFPE, SIGSEGV, SIGTERM are caught.
       */
      static void catchSignals();

      /**
       * Reset signal handlers to defaults.
       */
      static void resetSignals();

      /**
       * Generates and demangles stack trace.
       * On success it allocates a buffer via malloc where it stores null-byte-separated string
       * of frame names. The caller should free buffer.
       * On failure 0 is returned and bufferPtr is unchanged.
       * @param bufferPtr where to save pointer to buffer
       * @return number of frames
       */
      static int getStackTrace( char** bufferPtr );

      /**
       * Raise trap signal (to trigger breakpoint).
       */
      static void trap();

      /**
       * Print given error message.
       * Everything is printed to stderr and log file, if log target is a file.
       * @param msg
       */
      static void error( const char* msg, ... );

      /**
       * Crash with given message, stack trace and wait for a debugger or fatal signal.
       * Everything is printed to stderr and log file, if log target is a file.
       * @param msg
       */
      static void abort( const char* msg, ... );

  };

}
