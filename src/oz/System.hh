/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file oz/System.hh
 *
 * System class.
 */

#pragma once

#include "common.hh"

namespace pp
{

// Forward declarations for Native Client (NaCl) classes.
class Module;
class Instance;
class Core;

}

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

    /// Wait for CTRL-C (or other fatal signal) in <tt>abort()</tt>, so one has time to attach a
    /// debugger.
    static const int HALT_BIT = 0x01;

    /// Catch fatal signals (SIGILL, SIGABRT, SIGFPE and SIGSEGV), upon which print diagnostics and
    /// abort the program (similar to <tt>error()</tt> call).
    static const int SIGNAL_HANDLER_BIT = 0x10;

    /// Override handlers for exception violations (<tt>std::terminate()</tt> and
    /// <tt>std::unexpected()</tt>) with handlers that print diagnostics and abort the program via
    /// <tt>error()</tt> call.
    static const int EXCEPTION_HANDLERS_BIT = 0x20;

    /// Handlers bitmask.
    static const int HANDLERS_MASK = 0xf0;

  private:

    static System        system;   ///< Private instance, takes care for static (de)initialisation.

  public:

    static pp::Module*   module;   ///< NaCl module.
    static pp::Instance* instance; ///< NaCl instance.
    static pp::Core*     core;     ///< NaCl pp::Core interface.
    static int           width;    ///< Current width of NaCl module area in browser.
    static int           height;   ///< Current height of NaCl module in browser.

  private:


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
     * Trigger a breakpoint.
     *
     * It raises <tt>SIGTRAP</tt> on Linux or calls <tt>DebugBreak()</tt> on Windows.
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
    OZ_NORETURN
    OZ_PRINTF_FORMAT( 2, 3 )
    static void error( int nSkippedFrames, const char* msg, ... );

    /**
     * Initialise <tt>System</tt> features.
     *
     * Set-up crash handlers for cases specified in <tt>flags</tt>. If <tt>HALT_BIT</tt> is also
     * given, crash handlers wait for CTRL-C before exit.
     */
#if defined( NDEBUG ) || defined( __native_client__ )
    static void init( int flags = HANDLERS_MASK );
#else
    static void init( int flags = HANDLERS_MASK | HALT_BIT );
#endif

    /**
     * Deinitialise <tt>System</tt>.
     *
     * It resets signal handlers, <tt>std::terminate()</tt> and <tt>std::unexpected()</tt> to
     * defaults.
     */
    static void free();

};

}
