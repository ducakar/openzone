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
 * @file client/OpenAL.cpp
 */

#include "stable.hpp"

#include "client/OpenAL.hpp"

namespace oz
{
namespace client
{

#ifndef NDEBUG

void alCheckError( const char* file, int line, const char* function )
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
      message = "UNKNOWN(" + String( int( result ) ) + ")";
      break;
    }
  }

  System::trap();
  System::abort( "AL error `%s' at %s:%d: %s", message, file, line, function );
}

#endif

}
}
