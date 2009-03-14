/*
 *  O_Tree2.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.hpp"

#include "O_Tree2.hpp"

#include "World.hpp"

namespace oz
{

  const char *O_Tree2::NAME = "O_Tree2";
  const int O_Tree2::TYPE = String::hash( O_Tree2::NAME );

  Object *O_Tree2::build( const Vec3 &p )
  {
    return new O_Tree2( p );
  }

  O_Tree2::O_Tree2( const Vec3 &p_ )
  {
    p = p_,
    dim = Vec3( 0.6f, 0.6f, 2.25f );

    flags = Object::CLIP_BIT | Object::RELEASED_CULL_BIT;
    type = TYPE;

    model = translator.getModel( "tree3.md2" );
  }

  O_Tree2::O_Tree2( float x, float y )
  {
    p = Vec3( x, y, world.terrain.height( x, y ) + 1.2f ),
    dim = Vec3( 0.6f, 0.6f, 2.25f );

    flags = Object::CLIP_BIT | Object::RELEASED_CULL_BIT;
    type = TYPE;

    model = translator.getModel( "tree3.md2" );
  }

  void O_Tree2::onDestroy()
  {
    world.genParticles( 10, p, Vec3::zero(), 1.2f, 1.2f, 0.0f, 20.0f, 0.1f,
                        Vec3( 0.5f, 0.5f, 0.5f ), 0.2f );
    world.remove( this );
  }

  void O_Tree2::load( FILE *stream )
  {
    Object::load( stream );
  }

  void O_Tree2::save( FILE *stream )
  {
//     fwrite( NAME, sizeof( NAME ), 1, stream );

    Object::save( stream );
  }

}
