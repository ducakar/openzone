/*
 *  O_Tree1.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.hpp"

#include "O_Tree1.hpp"

#include "World.hpp"

namespace oz
{

  const char *O_Tree1::NAME = "O_Tree1";
  const int O_Tree1::TYPE = String::hash( O_Tree1::NAME );

  Object *O_Tree1::build( const Vec3 &p )
  {
    return new O_Tree1( p );
  }

  O_Tree1::O_Tree1( const Vec3 &p_ )
  {
    logFile.println( "%g %g %g", p_.x, p_.y, p_.z );
    p = p_,
    dim = Vec3( 0.7f, 0.7f, 3.8f );

    flags = Object::CLIP_BIT | Object::RELEASED_CULL_BIT;
    type = TYPE;

    damage = Math::INF;
  }

  O_Tree1::O_Tree1( float x, float y )
  {
    p = Vec3( x, y, world.terrain.height( x, y ) + 2.5f ),
    dim = Vec3( 0.7f, 0.7f, 3.8f );

    flags = Object::CLIP_BIT | Object::RELEASED_CULL_BIT;
    type = TYPE;

    damage = Math::INF;
  }

  void O_Tree1::onDestroy()
  {
    world.genParticles( 10, p, Vec3::zero(), 1.2f, 1.2f, 0.0f, 20.0f, 0.1f,
                        Vec3( 0.5f, 0.5f, 0.5f ), 0.2f );
    world.remove( this );
  }

  void O_Tree1::load( FILE *stream )
  {
    Object::load( stream );
  }

  void O_Tree1::save( FILE *stream )
  {
//     fwrite( NAME, sizeof( NAME ), 1, stream );

    Object::save( stream );
  }

}
