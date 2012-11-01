/*
 * ozCore - OpenZone Core Library.
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
 * @file ozCore/System.hh
 *
 * System class.
 */

#pragma once

#include "common.hh"

/**
 * @def OZ_WARNING
 * Wrapper for `System::warning()`, filling in the current function, file and line parameters.
 */
// FIXME Add a space after ellipsis once KDevelop gets that bug fixed.
#define OZ_WARNING( ...) \
  oz::System::warning( __PRETTY_FUNCTION__, __FILE__, __LINE__, 0, __VA_ARGS__ )

/**
 * @def OZ_ERROR
 * Wrapper for `System::error()`, filling in the current function, file and line parameters.
 */
// FIXME Add a space after ellipsis once KDevelop gets that bug fixed.
#define OZ_ERROR( ...) \
  oz::System::error( __PRETTY_FUNCTION__, __FILE__, __LINE__, 0, __VA_ARGS__ )

// Forward declarations for NaCl.
namespace pp
{

class Module;
class Instance;
class Core;

}

namespace oz
{

/**
 * Various utilities for issuing warnings, errors, intercepting fatal signals etc.
 */
class System
{
  public:

    /// Catch fatal signals (SIGQUIT, SIGILL, SIGABRT, SIGFPE and SIGSEGV), upon which print
    /// diagnostics and abort the program (similar to `error()` call).
    static const int SIGNALS_BIT = 0x01;

    /// Override handlers for exception violations (`std::terminate()` and `std::unexpected()`) with
    /// handlers that print diagnostics and abort the program via `error()` call.
    static const int EXCEPTIONS_BIT = 0x02;

    /// If running from a terminal, halt in `abort()`, so one has time to attach a debugger.
    static const int HALT_BIT = 0x04;

    /// Set system locale for the application (`setlocale( LC_ALL, "" )`).
    static const int LOCALE_BIT = 0x08;

    static pp::Module*   module;   ///< NaCl module.
    static pp::Instance* instance; ///< NaCl instance.
    static pp::Core*     core;     ///< NaCl `pp::Core` interface.

  public:

    /**
     * Trigger a breakpoint.
     *
     * It raises `SIGTRAP` on Linux or calls `DebugBreak()` on Windows.
     */
    static void trap();

    /**
     * Play a sound alert.
     *
     * Sine wave from `oz/bellSample.inc` is played asynchronously through platform's native sound
     * system (PulseAudio and ALSA on Linux, `PlaySound` system call on Windows).
     *
     * Bell depends upon several statically initialised structures. If it is called inside static
     * initialisation/deinitialisation before/after those are initialised/deinitialised, it won't
     * play.
     *
     * On NaCl, `System::instance` and `System::core` must be set properly for bell to work.
     */
    static void bell();

    /**
     * Print warning message.
     *
     * This function first triggers breakpoint with `trap()`, prints error message, file location
     * and stack trace (skipping `nSkippedFrames` stack frames relative to the caller) to log and
     * plays a bell.
     *
     * You will probably want to use `OZ_WARNING` macro instead to fill in the current function,
     * file and line for you.
     */
    OZ_PRINTF_FORMAT( 5, 6 )
    static void warning( const char* function, const char* file, int line, int nSkippedFrames,
                         const char* msg, ... );

    /**
     * Print error message and halt the program.
     *
     * Same as `System::warning()` but also aborts the program. If running from a terminal and
     * `HALT_BIT` was passed to `init()` initialisation, it halts before aborting so one can attach
     * a debugger.
     *
     * You will probably want to use `OZ_ERROR` macro instead to fill in the current function, file
     * and line for you.
     */
    OZ_NORETURN
    OZ_PRINTF_FORMAT( 5, 6 )
    static void error( const char* function, const char* file, int line, int nSkippedFrames,
                       const char* msg, ... );

    /**
     * Initialise current thread's signal handlers according to `flags` passed to `init()`.
     *
     * Signal handlers must be set-up for each thread in a process separately. `init()` method sets
     * them up for the caller thread only, for other threads this method should be used unless
     * created with `Thread::start()`, which implicitly calls this method.
     */
    static void threadInit();

    /**
     * Initialise `System` features.
     *
     * Set-up locale and crash handlers depending on `flags`. If `HALT_BIT` is also given, crash
     * handlers wait for CTRL-C before exit.
     */
#if defined( __ANDROID__ )
    static void init( int flags = SIGNALS_BIT | EXCEPTIONS_BIT );
#elif defined( __native_client__ )
    static void init( int flags = EXCEPTIONS_BIT );
#elif !defined( NDEBUG )
    static void init( int flags = SIGNALS_BIT | EXCEPTIONS_BIT | HALT_BIT | LOCALE_BIT );
#else
    static void init( int flags = SIGNALS_BIT | EXCEPTIONS_BIT | LOCALE_BIT );
#endif

    /**
     * Deinitialise `System`.
     *
     * It resets signal handlers, `std::terminate()` and `std::unexpected()` to defaults.
     */
    static void free();

};

}
