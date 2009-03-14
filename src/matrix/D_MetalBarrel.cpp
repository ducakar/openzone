/*
 *  D_MetalBarrel.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.hpp"

#include "D_MetalBarrel.hpp"

#include "World.hpp"

namespace oz
{

  const char *D_MetalBarrel::NAME = "D_MetalBarrel";
  const int D_MetalBarrel::TYPE = String::hash( D_MetalBarrel::NAME );

  Object *D_MetalBarrel::build( const Vec3 &p )
  {
    return new D_MetalBarrel( p );
  }

  D_MetalBarrel::D_MetalBarrel( const Vec3 &p_ )
  {
    p = p_,
    dim = Vec3( 0.4032f, 0.4032f, 0.5922f );

    flags = Object::DYNAMIC_BIT | Object::CLIP_BIT;
    type = TYPE;

    damage = Math::INF;

    velocity = Vec3::zero();
    mass = 50.0f;

    model = translator.getModel( "metalBarrel.md2" );
  }

  void D_MetalBarrel::onDestroy()
  {
    world.genParticles( 10, p, velocity, 1.2f, 1.2f, 0.0f, 20.0f, 0.1f,
                        Vec3( 0.5f, 0.5f, 0.5f ), 0.2f );

    world.remove( this );
  }

}
