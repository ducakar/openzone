/*
 *  System.hpp
 *
 *  Class for generating stack trace for the current function call.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
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

      static OZ_THREAD_LOCAL void* framePtrs[TRACE_SIZE + 1];
      static OZ_THREAD_LOCAL char  output[TRACE_BUFFER_SIZE];

      static bool isHaltEnabled;

      static void signalHandler( int signum );

    private:

      // singleton
      System();

    public:

      /**
       * If turned on, the signal handler will halt the program on a fatal signal, write a
       * notification to stderr and wait for another fatal signal (e.g. CTRL-C).
       * This is intended to halt the program on a crash, so one can attach with debugger and
       * inspect the program state.
       * Default is off.
       * @param value
       */
      static void enableHalt( bool value );

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
       * Raise trap signal (to trigger breakpoint).
       */
      static void trap();

      /**
       * Halt. Print a message to stdout and wait for a key to continue.
       */
      static void halt();

      /**
       * Print given error message.
       * Everything is printed to stderr and log file, if log target is a file.
       * @param msg
       */
      static void error( const char* msg, ... );

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
       * Crash with given message, stack trace and wait for a debugger or fatal signal.
       * Everything is printed to stderr and log file, if log target is a file.
       * @param msg
       */
      static void abort( const char* msg, ... );

  };

}
