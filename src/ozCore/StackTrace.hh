/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file ozCore/StackTrace.hh
 *
 * `StackTrace` class.
 */

#pragma once

#include "Thread.hh"

namespace oz
{

/**
 * Stack trace entry.
 *
 * @note
 * Stack trace retrieval is currently only implemented for glibc.
 */
class StackTrace
{
public:

  /// Maximum number of stack frames.
  static const int MAX_FRAMES = 32;

public:

  char  thread[Thread::NAME_LENGTH + 1]; ///< Current thread's name.
  int   nFrames;                         ///< Number of stack frames.
  void* frames[MAX_FRAMES];              ///< Pointers to stack frames.

public:

  /**
   * Generate `StackTrace` class containing data about the current stack and thread.
   *
   * @param nSkippedFrames number of stack frames to skip (must be >= 0).
   */
  static StackTrace current(int nSkippedFrames);

};

}
