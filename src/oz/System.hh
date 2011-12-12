/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
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

    static int initFlags; ///< Holds flags which components are initialised.

    /**
     * Signal handler.
     *
     * Handler prints information about signal and calls <code>System::abort()</code>.
     */
    static void signalHandler( int signum );

    /**
     * Set signal handlers.
     *
     * Set signal handlers to catch critical signals.
     * SIGINT, SIGQUIT, SIGILL, SIGABRT, SIGFPE, SIGSEGV and SIGTERM should be caught. Some
     * functions or errors may ruin signal catching.
     */
    static void catchSignals();

    /**
     * Reset signal handlers to defaults.
     */
    static void resetSignals();

    /**
     * Default constructor dynamically loads <tt>libpulse-simple</tt> to enable bell on Linux.
     */
    System();

    /**
     * Destructor delays process termination until bell sound is played to the end.
     */
    ~System();

  public:

    /**
     * Play a sound alert (asynchronously).
     *
     * On Linux, sine wave from <tt>oz/bellSample.inc</tt> is played through PulseAudio. On Windows,
     * default system bell is used.
     */
    static void bell();

    /**
     * Raise trap signal (to trigger a breakpoint).
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
     * Print error message.
     *
     * Everything is printed to stderr and log, unless log output is stdout.
     */
    OZ_PRINTF_FORMAT( 1, 2 )
    static void error( const char* msg, ... );

    /**
     * Abort program.
     *
     * Print the error message and stack trace, call <code>halt()</code> (if HALT_BIT was passed on
     * initialisation) and terminate program with SIGABRT after that.
     *
     * Everything is printed to stderr and log, unless log output is stdout.
     */
    OZ_PRINTF_FORMAT( 1, 2 )
    static void abort( const char* msg, ... );

    /**
     * Initialise <tt>System</tt> features.
     *
     * Set-up crash handler if <tt>CATCH_SIGNALS_BIT</tt> is given. If <tt>HALT_BIT</tt> is also
     * given, crash handler waits for CTRL-C before exit.
     */
#ifdef NDEBUG
    static void init( int flags = CATCH_SIGNALS_BIT | HALT_BIT );
#else
    static void init( int flags = CATCH_SIGNALS_BIT );
#endif

};

}
