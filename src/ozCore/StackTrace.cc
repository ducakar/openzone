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

#include "StackTrace.hh"

#include "Arrays.hh"

#include <cstring>

#ifdef __GLIBC__
# include <execinfo.h>
#endif

namespace oz
{

const int StackTrace::MAX_FRAMES;

StackTrace StackTrace::current(int nSkippedFrames)
{
  OZ_ASSERT(nSkippedFrames >= 0);

  StackTrace st = {{}, 0, {}};
  memccpy(st.thread, Thread::name(), '\0', Thread::NAME_LENGTH);

#ifdef __GLIBC__

  void* framesBuffer[MAX_FRAMES + 4];
  int   nBufferedFrames = backtrace(framesBuffer, MAX_FRAMES + 4);

  st.nFrames = clamp<int>(nBufferedFrames - 1 - nSkippedFrames, 0, MAX_FRAMES);
  Arrays::copy<void*>(framesBuffer + 1 + nSkippedFrames, st.nFrames, st.frames);

#endif
  return st;
}

}
