/*
 *  System.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file oz/System.hpp
 */

#include "common.hpp"

namespace oz
{

/**
 * Class for generating stack trace for the current function call.
 *
 * @ingroup oz
 */
class System
{
  private:

    /// Maximum number of stack frames to be recorded.
    static const int TRACE_SIZE = 16;

    /// Size of the <tt>output</tt> buffer.
    static const int TRACE_BUFFER_SIZE = 4096;

    /// Size of buffer for demangling function name.
    static const int STRING_BUFFER_SIZE = 1024;

    /// Table of signal names and descriptions.
    static const char* const SIGNALS[][2];

    /// Internal buffer to store stack frames.
    static OZ_THREAD_LOCAL void* framePtrs[System::TRACE_SIZE + 1];

    /// Internal buffer for generation of stack trace string.
    static OZ_THREAD_LOCAL char output[TRACE_BUFFER_SIZE];

    /// Whether to enable <code>halt()</code> to be called by <code>abort()</code>.
    static bool isHaltEnabled;

    /**
     * Signal handler, prints signal info and calls <code>abort()</code>.
     */
    static void signalHandler( int signum );

  public:

    /**
     * Singleton.
     */
    System() = delete;

    /**
     * Enable halt on crash.
     *
     * If turned on, the signal handler will halt the program on a fatal signal, write a
     * notification to stderr and wait for another fatal signal (e.g. CTRL-C).
     * This is intended to halt the program on a crash, so one can attach with debugger and
     * inspect the program state.
     * Default is off.
     */
    static void enableHalt( bool value );

    /**
     * Set signal handlers.
     *
     * Set signal handlers to catch critical signals. Handlers print information about signal,
     * and then call <code>abort()</code>.
     * SIGINT, SIGQUIT, SIGILL, SIGABRT, SIGFPE, SIGSEGV and SIGTERM should be caught. Some
     * functions or errors may ruin signal catching.
     */
    static void catchSignals();

    /**
     * Reset signal handlers to defaults.
     */
    static void resetSignals();

    /**
     * Raise trap signal (to trigger a breakpoint).
     */
    static void trap();

    /**
     * Wait for a key to continue or a fatal signal.
     *
     * This function is intended to halt program when something goes wrong, so one can attach
     * a debugger.
     */
    static void halt();

    /**
     * Print error message.
     *
     * Everything is printed to stderr and log, unless log output is stdout.
     */
    static void error( const char* msg, ... );

    /**
     * Generates and demangles stack trace.
     *
     * On success it allocates a buffer via <tt>malloc</tt> where it stores null-byte-separated
     * strings of frame names. The caller should free buffer.
     * On failure 0 is returned and <tt>bufferPtr</tt> is unchanged.
     *
     * @param bufferPtr where to save pointer to the buffer.
     * @return Number of frames.
     */
    static int getStackTrace( char** bufferPtr );

    /**
     * Abort program.
     *
     * Print the error message and stack trace, call <code>halt()</code> and terminate program with
     * SIGABRT after that. Everything is printed to stderr and log, unless log output is stdout.
     */
    static void abort( const char* msg, ... );

};

}
