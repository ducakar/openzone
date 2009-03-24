/*
 *  DynObject.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "DynObject.h"

namespace oz
{

  void DynObject::load( FILE *stream )
  {
    Object::load( stream );

    fread( &velocity, sizeof( velocity ), 1, stream );
    fread( &newVelocity, sizeof( newVelocity ), 1, stream );
    fread( &mass, sizeof( mass ), 1, stream );
    fread( &lower, sizeof( lower ), 1, stream );
    fread( &floor, sizeof( floor ), 1, stream );

    lower = SDL_SwapBE32( lower );
  }

  void DynObject::save( FILE *stream )
  {
    Object::load( stream );

    lower = SDL_SwapBE32( lower );

    fwrite( &velocity, sizeof( velocity ), 1, stream );
    fwrite( &newVelocity, sizeof( newVelocity ), 1, stream );
    fwrite( &mass, sizeof( mass ), 1, stream );
    fwrite( &lower, sizeof( lower ), 1, stream );
    fwrite( &floor, sizeof( floor ), 1, stream );

    lower = SDL_SwapBE32( lower );
  }

}
