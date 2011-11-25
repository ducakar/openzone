/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
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
 * @file oz/System.hpp
 */

#pragma once

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

    /// Table of signal names and descriptions.
    static const char* const SIGNALS[][2];

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
     * Play a sound alert.
     *
     * Runs <tt>paplay /usr/share/sounds/pop.wav &</tt>. <tt>paplay</tt> is part of PulseAudio
     * server and <tt>pop.wav</tt> comes with KDE SC.
     */
    static void bell();

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
    OZ_PRINTF_FORMAT( 1, 2 )
    static void error( const char* msg, ... );

    /**
     * Abort program.
     *
     * Print the error message and stack trace, call <code>halt()</code> and terminate program with
     * SIGABRT after that. Everything is printed to stderr and log, unless log output is stdout.
     */
    OZ_PRINTF_FORMAT( 1, 2 )
    static void abort( const char* msg, ... );

};

}
