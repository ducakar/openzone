/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file ozEngine/OpenAL.cc
 */

#include "OpenAL.hh"

namespace oz
{

#ifndef NDEBUG

void alCheckError( const char* function, const char* file, int line )
{
  const char* message;
  ALenum result = alGetError();

  switch( result ) {
    case AL_NO_ERROR: {
      return;
    }
    case AL_INVALID_NAME: {
      message = "AL_INVALID_NAME";
      break;
    }
    case AL_INVALID_ENUM: {
      message = "AL_INVALID_ENUM";
      break;
    }
    case AL_INVALID_VALUE: {
      message = "AL_INVALID_VALUE";
      break;
    }
    case AL_INVALID_OPERATION: {
      message = "AL_INVALID_OPERATION";
      break;
    }
    case AL_OUT_OF_MEMORY: {
      message = "AL_OUT_OF_MEMORY";
      break;
    }
    default: {
      message = String::str( "UNKNOWN(%d)", int( result ) );
      break;
    }
  }

  System::error( function, file, line, 1, "AL error '%s'", message );
}

#endif

}
