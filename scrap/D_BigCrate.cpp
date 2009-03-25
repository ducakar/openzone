/*
 *  D_BigCrate.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "D_BigCrate.h"

#include "World.h"

namespace oz
{

  const char *D_BigCrate::NAME = "D_BigCrate";
  const int D_BigCrate::TYPE = String::hash( D_BigCrate::NAME );

  Object *D_BigCrate::build( const Vec3 &p, Object *content )
  {
    return new D_BigCrate( p, content );
  }

  D_BigCrate::D_BigCrate( const Vec3 &p_, Object *content_ )
  {
    p = p_,
    dim = Vec3( 0.6f, 0.6f, 0.6f );

    flags = Object::DYNAMIC_BIT | Object::CLIP_BIT;
    type = null;

    damage = Math::INF;

    velocity = Vec3::zero();
    mass = 150.0f;

    content = content_;
  }

  D_BigCrate::~D_BigCrate()
  {
    if( content != null ) {
      delete content;
    }
  }

  void D_BigCrate::onDestroy()
  {
    world.genParticles( 10, p, velocity, 1.2f, 1.2f, 0.0f, 20.0f, 0.1f,
                        Vec3( 0.5f, 0.5f, 0.5f ), 0.2f );

    if( content != null ) {
      content->p = p;
      world.add( content );
      content = null;
    }
    world.remove( this );
  }

}
