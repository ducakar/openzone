/*
 *  OpenAL.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/OpenAL.hpp"

namespace oz
{
namespace client
{

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

}
}
