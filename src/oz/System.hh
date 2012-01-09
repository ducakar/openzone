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

    /// Wait for a CTRL-C in <tt>abort()</tt>, so one has time to attach debugger.
    static const int HALT_BIT = 0x02;

    /// Catch fatal signals, print diagnostics and abort program.
    static const int SIGNAL_HANDLER_BIT = 0x10;

    /// Override handlers for exception violations (<tt>std::terminate()</tt> and
    /// <tt>std::unexpected()</tt>).
    static const int EXCEPTION_HANDLERS_BIT = 0x20;

    /// Handlers bitmask.
    static const int HANDLERS_MASK = 0xf0;

  private:

    static System system; ///< Private instance, takes care for static initialisation/destruction.

    /**
     * Sets up bell and disables <tt>SIGTRAP</tt> handler on Linux, since the default handler would
     * crash the application on <tt>trap()</tt> call.
     */
    System();

    /**
     * Destructor delays normal process termination until the bell finishes playing.
     */
    ~System();

  public:

    /**
     * Play a sound alert.
     *
     * Sine wave from <tt>oz/bellSample.inc</tt> is played asynchronously through PulseAudio on
     * Linux or Win32 API on Windows.
     */
    static void bell();

    /**
     * Trigger a breakpoint (raises <tt>SIGTRAP</tt> on Linux or calls DebugBreak() on Windows).
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
     *
     * If <tt>nSkippedFrames</tt> is 0, stack frames from including caller frame are included. Can
     * also be negative; -1 and -2 include from <tt>error()</tt>'s and <tt>System::current()</tt>'s
     * frame respectively.
     */
    OZ_PRINTF_FORMAT( 2, 3 )
    static void error( int nSkippedFrames, const char* msg, ... );

    /**
     * Abort program.
     */
    static void abort();

    /**
     * Initialise <tt>System</tt> features.
     *
     * Set-up crash handler for cases specified in <tt>flags</tt>. If <tt>HALT_BIT</tt> is also
     * given, crash handler waits for CTRL-C before exit.
     */
#ifdef NDEBUG
    static void init( int flags = HANDLERS_MASK );
#else
    static void init( int flags = HANDLERS_MASK | HALT_BIT );
#endif

    /**
     * Deinitialise <tt>System</tt>.
     *
     * It resets signal handlers (including <tt>SIG_IGN</tt>), <tt>std::terminate()</tt> and
     * <tt>std::unexpected()</tt> to defaults.
     */
    static void free();

};

}
