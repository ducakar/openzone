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

  ALenum alGetError()
  {
    ALenum result = ::alGetError();

    if( result != AL_NO_ERROR ) {
      switch( result ) {
        case AL_INVALID_NAME: {
          log.println( "AL: AL_INVALID_NAME" );
          break;
        }
        case AL_INVALID_ENUM: {
          log.println( "AL: AL_INVALID_ENUM" );
          break;
        }
        case AL_INVALID_VALUE: {
          log.println( "AL: AL_INVALID_VALUE" );
          break;
        }
        case AL_INVALID_OPERATION: {
          log.println( "AL: AL_INVALID_OPERATION" );
          break;
        }
        case AL_OUT_OF_MEMORY: {
          log.println( "AL: AL_OUT_OF_MEMORY" );
          break;
        }
      }
    }

    return result;
  }

}
}
