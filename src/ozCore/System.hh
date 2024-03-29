/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2019 Davorin Učakar
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
 * `System` class.
 */

#pragma once

#include "common.hh"

/**
 * @def OZ_ASSERT
 * If condition fails, raise SIGTRAP, print error using global log and abort program.
 */
#ifdef NDEBUG
# define OZ_ASSERT(cond) void(0)
#else
# define OZ_ASSERT(cond) \
  ((cond) ? \
   void(0) : oz::System::error(__PRETTY_FUNCTION__, __FILE__, __LINE__, 0, \
                               "Assertion `%s' failed.", #cond))
#endif

/**
 * @def OZ_ERROR
 * Wrapper for `System::error()`, filling in the current function, file and line parameters.
 */
#define OZ_ERROR(...) \
  oz::System::error(__PRETTY_FUNCTION__, __FILE__, __LINE__, 0, __VA_ARGS__)

namespace oz
{

/**
 * Various utilities for issuing warnings, errors, intercepting fatal signals etc.
 */
class System
{
public:

  /// Install signal handler.
  static constexpr int HANDLER_BIT = 0x01;

  /// If running from a terminal, the crash handler will wait for Enter before termination.
  static constexpr int HALT_BIT = 0x02;

  /// Default set of bits.
#ifdef NDEBUG
  static constexpr int DEFAULT_MASK = HANDLER_BIT;
#else
  static constexpr int DEFAULT_MASK = HANDLER_BIT | HALT_BIT;
#endif

  /// Type for crash handler function passed to `System::init()`.
  using CrashHandler = void ();

public:

  /**
   * Static class.
   */
  System() = delete;

  /**
   * Trigger a breakpoint.
   *
   * It raises `SIGTRAP` on Linux or calls `DebugBreak()` on Windows.
   */
  static void trap();

  /**
   * Play a sound alert.
   *
   * Sine wave with decreasing volume lasting ~0.3 s is played asynchronously through platform's
   * native sound system (PulseAudio on Linux, `PlaySound` system call on Windows and Pepper API on
   * NaCl).
   */
  static void bell();

  /**
   * Print error message and halt the program.
   *
   * Same as `System::warning()` but also aborts the application. If running from a terminal and
   * `HALT_BIT` was passed to `System::init()` initialisation, it halts before aborting so one can
   * attach a debugger.
   *
   * You will probably want to use `OZ_ERROR` macro instead to fill in the current function, file
   * and line for you.
   */
  OZ_NORETURN
  OZ_PRINTF_FORMAT(5, 6)
  static void error(const char* function, const char* file, int line, int nSkippedFrames,
                    const char* message, ...);

  /**
   * Initialise `System`.
   *
   * @param flags is a bitwise OR of the following bits:
   * - `HANDLER_BIT`: Catch fatal signals (SIGQUIT, SIGILL, SIGABRT, SIGFPE and SIGSEGV), upon
   *   which print diagnostics and abort the program (similar to `System::error()` function).
   * - `HALT_BIT`: If running from a terminal, previous handlers wait for user to press Enter before
   *   terminating the process via `System::abort()`, so one have time to attach a debugger.
   *
   * @param handler user-provided function called when the application is aborted by a signal
   *        `System::error()`. If non-null, it is invoked after the stack trace is printed.
   */
  static void init(int flags = DEFAULT_MASK, CrashHandler* handler = nullptr);

};

}
