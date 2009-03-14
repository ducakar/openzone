/*
 *  Shape.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.hpp"

#include "Shape.hpp"

namespace oz
{
namespace Client
{

  Shape shape;

  uint Shape::genRandomTetrahedicParticle( uint list, float size )
  {
    static const float SQRT_3_THIRDS = Math::sqrt( 3.0f ) / 3.0f;

    float dim = size / 2.0f;

    Vec3 v0 = Math::frand() * dim * Vec3( 0.0f,            0.0f,        1.0f );
    Vec3 v1 = Math::frand() * dim * Vec3( 0.0f,            2.0f / 3.0f, 0.0f );
    Vec3 v2 = Math::frand() * dim * Vec3( -SQRT_3_THIRDS, -1.0f / 3.0f, 0.0f );
    Vec3 v3 = Math::frand() * dim * Vec3(  SQRT_3_THIRDS, -1.0f / 3.0f, 0.0f );

    glNewList( list, GL_COMPILE );

    glBegin( GL_TRIANGLES );
      // fore
      glNormal3fv( ( ( v2 - v1 ) ^ ( v0 - v1 ) ).norm() );
      glVertex3fv( v0 );
      glVertex3fv( v1 );
      glVertex3fv( v2 );

      // left
      glNormal3fv( ( ( v1 - v3 ) ^ ( v0 - v3 ) ).norm() );
      glVertex3fv( v0 );
      glVertex3fv( v3 );
      glVertex3fv( v1 );

      // right
      glNormal3fv( ( ( v3 - v2 ) ^ ( v0 - v2 ) ).norm() );
      glVertex3fv( v0 );
      glVertex3fv( v2 );
      glVertex3fv( v3 );

      // bottom
      glNormal3fv( ( ( v3 - v1 ) ^ ( v2 - v1 ) ).norm() );
      glVertex3fv( v1 );
      glVertex3fv( v3 );
      glVertex3fv( v2 );
    glEnd();

    glEndList();

    return list;
  }

  uint Shape::genRandomCubicParticle( uint list, float size )
  {
    float dim = size / 2.0f;

    Vec3 v0( -dim * Math::frand(), -dim * Math::frand(), -dim );
    Vec3 v1(  dim * Math::frand(), -dim * Math::frand(), -dim );
    Vec3 v2(  dim * Math::frand(),  dim * Math::frand(), -dim );
    Vec3 v3( -dim * Math::frand(),  dim * Math::frand(), -dim );

    Vec3 v4( v0.x, v0.y,  dim );
    Vec3 v5( v1.x, v1.y,  dim );
    Vec3 v6( v2.x, v2.y,  dim );
    Vec3 v7( v3.x, v3.y,  dim );

    glNewList( list, GL_COMPILE );

    glBegin( GL_QUADS );
      // top
      glNormal3f( 0.0f, 0.0f, 1.0f );
      glVertex3fv( v4 );
      glVertex3fv( v5 );
      glVertex3fv( v6 );
      glVertex3fv( v7 );

      // fore
      glNormal3fv( ( ( v1 - v0 ) ^ ( v4 - v0 ) ).norm() );
      glVertex3fv( v0 );
      glVertex3fv( v1 );
      glVertex3fv( v5 );
      glVertex3fv( v4 );

      // left
      glNormal3fv( ( ( v0 - v3 ) ^ ( v7 - v3 ) ).norm() );
      glVertex3fv( v3 );
      glVertex3fv( v0 );
      glVertex3fv( v4 );
      glVertex3fv( v7 );

      // back
      glNormal3fv( ( ( v3 - v2 ) ^ ( v6 - v2 ) ).norm() );
      glVertex3fv( v2 );
      glVertex3fv( v3 );
      glVertex3fv( v7 );
      glVertex3fv( v6 );

      // right
      glNormal3fv( ( ( v2 - v1 ) ^ ( v5 - v1 ) ).norm() );
      glVertex3fv( v1 );
      glVertex3fv( v2 );
      glVertex3fv( v6 );
      glVertex3fv( v5 );

      // bottom
      glNormal3f( 0.0f, 0.0f, -1.0f );
      glVertex3fv( v3 );
      glVertex3fv( v2 );
      glVertex3fv( v1 );
      glVertex3fv( v0 );
    glEnd();

    glEndList();

    return list;
  }

  uint Shape::genBox( uint list, const AABB &bb, uint texture )
  {
    Vec3 v0 = -bb.dim;
    Vec3 v1 = +bb.dim;

    glNewList( list, GL_COMPILE );

    glBindTexture( GL_TEXTURE_2D, texture );
    glBegin( GL_QUADS );
      // top
      glNormal3f( 0.0f, 0.0f, 1.0f );
      glTexCoord2f( 0, 0 );
      glVertex3f( v0.x, v0.y, v1.z );
      glTexCoord2f( 1, 0 );
      glVertex3f( v1.x, v0.y, v1.z );
      glTexCoord2f( 1, 1 );
      glVertex3f( v1.x, v1.y, v1.z );
      glTexCoord2f( 0, 1 );
      glVertex3f( v0.x, v1.y, v1.z );

      // fore
      glNormal3f( 0.0f, -1.0f, 0.0f );
      glTexCoord2f( 0, 0 );
      glVertex3f( v0.x, v0.y, v0.z );
      glTexCoord2f( 1, 0 );
      glVertex3f( v1.x, v0.y, v0.z );
      glTexCoord2f( 1, 1 );
      glVertex3f( v1.x, v0.y, v1.z );
      glTexCoord2f( 0, 1 );
      glVertex3f( v0.x, v0.y, v1.z );

      // left
      glNormal3f( -1.0f, 0.0f, 0.0f );
      glTexCoord2f( 0, 0 );
      glVertex3f( v0.x, v1.y, v0.z );
      glTexCoord2f( 1, 0 );
      glVertex3f( v0.x, v0.y, v0.z );
      glTexCoord2f( 1, 1 );
      glVertex3f( v0.x, v0.y, v1.z );
      glTexCoord2f( 0, 1 );
      glVertex3f( v0.x, v1.y, v1.z );

      // back
      glNormal3f( 0.0f, 1.0f, 0.0f );
      glTexCoord2f( 0, 0 );
      glVertex3f( v1.x, v1.y, v0.z );
      glTexCoord2f( 1, 0 );
      glVertex3f( v0.x, v1.y, v0.z );
      glTexCoord2f( 1, 1 );
      glVertex3f( v0.x, v1.y, v1.z );
      glTexCoord2f( 0, 1 );
      glVertex3f( v1.x, v1.y, v1.z );

      // right
      glNormal3f( 1.0f, 0.0f, 0.0f );
      glTexCoord2f( 0, 0 );
      glVertex3f( v1.x, v0.y, v0.z );
      glTexCoord2f( 1, 0 );
      glVertex3f( v1.x, v1.y, v0.z );
      glTexCoord2f( 1, 1 );
      glVertex3f( v1.x, v1.y, v1.z );
      glTexCoord2f( 0, 1 );
      glVertex3f( v1.x, v0.y, v1.z );

      // bottom
      glNormal3f( 0.0f, 0.0f, -1.0f );
      glTexCoord2f( 0, 0 );
      glVertex3f( v0.x, v1.y, v0.z );
      glTexCoord2f( 1, 0 );
      glVertex3f( v1.x, v1.y, v0.z );
      glTexCoord2f( 1, 1 );
      glVertex3f( v1.x, v0.y, v0.z );
      glTexCoord2f( 0, 1 );
      glVertex3f( v0.x, v0.y, v0.z );
    glEnd();

    glEndList();

    return list;
  }

  void Shape::drawBox( const AABB &bb )
  {
    Vec3 v0 = -bb.dim;
    Vec3 v1 = +bb.dim;

    glBegin( GL_QUADS );
      // top
      glNormal3f( 0.0f, 0.0f, 1.0f );
      glTexCoord2f( 0, 0 );
      glVertex3f( v0.x, v0.y, v1.z );
      glTexCoord2f( 1, 0 );
      glVertex3f( v1.x, v0.y, v1.z );
      glTexCoord2f( 1, 1 );
      glVertex3f( v1.x, v1.y, v1.z );
      glTexCoord2f( 0, 1 );
      glVertex3f( v0.x, v1.y, v1.z );

      // fore
      glNormal3f( 0.0f, -1.0f, 0.0f );
      glTexCoord2f( 0, 0 );
      glVertex3f( v0.x, v0.y, v0.z );
      glTexCoord2f( 1, 0 );
      glVertex3f( v1.x, v0.y, v0.z );
      glTexCoord2f( 1, 1 );
      glVertex3f( v1.x, v0.y, v1.z );
      glTexCoord2f( 0, 1 );
      glVertex3f( v0.x, v0.y, v1.z );

      // left
      glNormal3f( -1.0f, 0.0f, 0.0f );
      glTexCoord2f( 0, 0 );
      glVertex3f( v0.x, v1.y, v0.z );
      glTexCoord2f( 1, 0 );
      glVertex3f( v0.x, v0.y, v0.z );
      glTexCoord2f( 1, 1 );
      glVertex3f( v0.x, v0.y, v1.z );
      glTexCoord2f( 0, 1 );
      glVertex3f( v0.x, v1.y, v1.z );

      // back
      glNormal3f( 0.0f, 1.0f, 0.0f );
      glTexCoord2f( 0, 0 );
      glVertex3f( v1.x, v1.y, v0.z );
      glTexCoord2f( 1, 0 );
      glVertex3f( v0.x, v1.y, v0.z );
      glTexCoord2f( 1, 1 );
      glVertex3f( v0.x, v1.y, v1.z );
      glTexCoord2f( 0, 1 );
      glVertex3f( v1.x, v1.y, v1.z );

      // right
      glNormal3f( 1.0f, 0.0f, 0.0f );
      glTexCoord2f( 0, 0 );
      glVertex3f( v1.x, v0.y, v0.z );
      glTexCoord2f( 1, 0 );
      glVertex3f( v1.x, v1.y, v0.z );
      glTexCoord2f( 1, 1 );
      glVertex3f( v1.x, v1.y, v1.z );
      glTexCoord2f( 0, 1 );
      glVertex3f( v1.x, v0.y, v1.z );

      // bottom
      glNormal3f( 0.0f, 0.0f, -1.0f );
      glTexCoord2f( 0, 0 );
      glVertex3f( v0.x, v1.y, v0.z );
      glTexCoord2f( 1, 0 );
      glVertex3f( v1.x, v1.y, v0.z );
      glTexCoord2f( 1, 1 );
      glVertex3f( v1.x, v0.y, v0.z );
      glTexCoord2f( 0, 1 );
      glVertex3f( v0.x, v0.y, v0.z );
    glEnd();

    glEndList();
  }

  void Shape::init()
  {
    uint spark = context.genList();

    genRandomTetrahedicParticle( spark, 0.5f );
  }

  void free()
  {}

}
}
