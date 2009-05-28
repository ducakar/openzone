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

  Object::~Object()
  {
    assert( dim.x <= AABB::REAL_MAX_DIMXY );
    assert( dim.y <= AABB::REAL_MAX_DIMXY );

    events.free();
    effects.free();
  }

  void Object::onUpdate()
  {}

  void Object::onHit( const Hit*, float )
  {}

  void Object::onDestroy()
  {}

  void Object::onUse( Object* )
  {}

  void Object::onPut()
  {}

  void Object::onCut()
  {}

  void Object::readUpdate( Net::Packet *packet )
  {

//     fread( &p, sizeof( p ), 1, stream );
//     fread( &dim, sizeof( dim ), 1, stream );
//     fread( &index, sizeof( index ), 1, stream );
//     fread( &flags, sizeof( flags ), 1, stream );
//     fread( &rotZ, sizeof( rotZ ), 1, stream );
//
//     index = SDL_SwapBE32( index );
//     flags = SDL_SwapBE32( flags );
  }

  void Object::writeUpdate( Net::Packet *packet )
  {
//     index = SDL_SwapBE32( index );
//     flags = SDL_SwapBE32( flags );
//
//     fwrite( &p, sizeof( p ), 1, stream );
//     fwrite( &dim, sizeof( dim ), 1, stream );
//     fwrite( &index, sizeof( index ), 1, stream );
//     fwrite( &flags, sizeof( flags ), 1, stream );
//     fwrite( &type, sizeof( type ), 1, stream );
//     fwrite( &rotZ, sizeof( rotZ ), 1, stream );
//
//     index = SDL_SwapBE32( index );
//     flags = SDL_SwapBE32( flags );
  }

}
