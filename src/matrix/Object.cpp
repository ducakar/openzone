/*
 *  Object.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Object.h"

namespace oz
{

  void Object::load( FILE *stream )
  {
    fread( &p, sizeof( p ), 1, stream );
    fread( &dim, sizeof( dim ), 1, stream );
    fread( &index, sizeof( index ), 1, stream );
    fread( &flags, sizeof( flags ), 1, stream );
    fread( &type, sizeof( type ), 1, stream );
    fread( &rotZ, sizeof( rotZ ), 1, stream );

    index = SDL_SwapBE32( index );
    flags = SDL_SwapBE32( flags );
    type  = SDL_SwapBE32( type );
  }

  void Object::save( FILE *stream )
  {
    index = SDL_SwapBE32( index );
    flags = SDL_SwapBE32( flags );
    type  = SDL_SwapBE32( type );

    fwrite( &p, sizeof( p ), 1, stream );
    fwrite( &dim, sizeof( dim ), 1, stream );
    fwrite( &index, sizeof( index ), 1, stream );
    fwrite( &flags, sizeof( flags ), 1, stream );
    fwrite( &type, sizeof( type ), 1, stream );
    fwrite( &rotZ, sizeof( rotZ ), 1, stream );

    index = SDL_SwapBE32( index );
    flags = SDL_SwapBE32( flags );
    type  = SDL_SwapBE32( type );
  }

}
