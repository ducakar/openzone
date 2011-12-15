/*
 * liboz - OpenZone core library.
 *
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file oz/System.hh
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Class for generating stack trace for the current function call.
 *
 * @ingroup oz
 */
class System
{
  public:

    /// Catch fatal signals and print stack trace.
    static const int CATCH_SIGNALS_BIT = 0x01;

    /// Wait for a CTRL-C when a fatal signal is caught, so one has time to attach debugger.
    static const int HALT_BIT = 0x02;

  private:

    static System system; ///< Private instance, takes care for static initialisation/destruction.

    /**
     * Default constructor disables handler for SIGTRAP on Linux.
     *
     * liboz triggers breakpoint on exceptions and failed assertions, which is done by raising
     * SIGTRAP on Linux or <tt>DebugBreak()</tt> call on Windows. However, default handler for
     * SIGTRAP crashes the process, so it's a good idea to disable it.
     */
    System();

    /**
     * Destructor delays normal process termination until bell finishes playing.
     */
    ~System();

  public:

    /**
     * Play a sound alert (asynchronously).
     *
     * Sine wave from <tt>oz/bellSample.inc</tt> is played through PulseAudio on Linux or Win32 API
     * in Windows.
     */
    static void bell();

    /**
     * Trigger a breakpoint (raises SIGTRAP on Linux or calls DebugBreak() on Windows).
     */
    static void trap();

    /**
     * Wait for a key or a fatal signal to continue.
     *
     * This function is intended to halt program when something goes wrong, so one can attach
     * a debugger.
     */
    static void halt();

    /**
     * Print error message and stack trace.
     */
    OZ_PRINTF_FORMAT( 1, 2 )
    static void error( const char* msg, ... );

    /**
     * Abort program.
     *
     * If <tt>halt</tt> is true and <tt>HALT_BIT</tt> has been passed on initialisation, call
     * <code>halt()</code>, then terminate program with SIGABRT.
     */
    static void abort( bool halt = true );

    /**
     * Initialise <tt>System</tt> features.
     *
     * Set-up crash handler if <tt>CATCH_SIGNALS_BIT</tt> is given. If <tt>HALT_BIT</tt> is also
     * given, crash handler waits for CTRL-C before exit.
     */
#ifdef NDEBUG
    static void init( int flags = CATCH_SIGNALS_BIT );
#else
    static void init( int flags = CATCH_SIGNALS_BIT | HALT_BIT );
#endif

};

}
