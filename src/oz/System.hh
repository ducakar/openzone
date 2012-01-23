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
    static const int HALT_BIT = 0x01;

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
     * Abort program.
     *
     * If <tt>HALT_BIT</tt> was passed in on initialisation and <tt>preventHalt</tt> is false,
     * program is halted and waits for a fatal signal before it is aborted, so a debugger can be
     * attached.
     */
    OZ_NORETURN
    static void abort( bool preventHalt = false );

    /**
     * Trigger a breakpoint (raises <tt>SIGTRAP</tt> on Linux or calls DebugBreak() on Windows).
     */
    static void trap();

    /**
     * Play a sound alert.
     *
     * Sine wave from <tt>oz/bellSample.inc</tt> is played asynchronously through PulseAudio on
     * Linux or Win32 API on Windows.
     */
    static void bell();

    /**
     * Print warning message.
     *
     * This function first triggers breakpoint with <tt>trap()</tt>, prints error message and stack
     * trace to global log instance and plays a bell.
     */
    OZ_PRINTF_FORMAT( 2, 3 )
    static void warning( int nSkippedFrames, const char* msg, ... );

    /**
     * Print error message and halt the program.
     *
     * Same as <tt>warning()</tt> but also aborts the program.
     */
    OZ_PRINTF_FORMAT( 2, 3 ) OZ_NORETURN
    static void error( int nSkippedFrames, const char* msg, ... );

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
