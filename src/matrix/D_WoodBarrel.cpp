/*
 *  D_WoodBarrel.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "D_WoodBarrel.h"

#include "World.h"

namespace oz
{

  const char *D_WoodBarrel::NAME = "D_WoodBarrel";
  const int D_WoodBarrel::TYPE = String::hash( D_WoodBarrel::NAME );

  Object *D_WoodBarrel::build( const Vec3 &p, Object *content )
  {
    return new D_WoodBarrel( p, content );
  }

  D_WoodBarrel::D_WoodBarrel( const Vec3 &p_, Object *content_ )
  {
    p = p_,
    dim = Vec3( 0.35f, 0.35f, 0.482f );

    flags = Object::DYNAMIC_BIT | Object::CLIP_BIT;
    type = TYPE;

    damage = Math::INF;

    velocity = Vec3::zero();
    mass = 50.0f;

    content = content_;
  }

  D_WoodBarrel::~D_WoodBarrel()
  {
    if( content != null ) {
      delete content;
    }
  }

  void D_WoodBarrel::onDestroy()
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
