/*
 *  StackTrace.hpp
 *
 *  Class for generating stack trace for the current function call.
 *  This class only has effect if OZ_ENABLE_STACKTRACE is enabled.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "common.hpp"

namespace oz
{

  class StackTrace
  {
    public:

      static const int BACKTRACE_SIZE        = 16;
      static const int BACKTRACE_BUFFER_SIZE = 4096;
      static const int STRING_BUFFER_SIZE    = 1024;

      /**
       * Generates and demangles stack trace.
       * On success it allocates a buffer via malloc where it stores null-byte-separated string
       * of frame names. The caller should free buffer.
       * On failure 0 is returned and bufferPtr is unchanged.
       * @param bufferPtr where to save pointer to buffer
       * @return number of frames
       */
      static int get( char** bufferPtr );

  };

}
