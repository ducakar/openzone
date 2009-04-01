/*
 *  Water.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Water.h"

#include "Context.h"

namespace oz
{
namespace client
{

  Water water;

  const float Water::TEX_STRETCH = 8.0f;
  const float Water::TEX_BIAS = 0.5f;

  const float Water::ALPHA = 0.75f;

  const float Water::COLOR[] = { 0.0f, 0.05f, 0.25f, 1.0f };
  const float Water::VISIBILITY = 8.0f;

  void Water::init()
  {
    texture = context.loadTexture( "tex/water1.jpg", true );
  }

  void Water::update()
  {
    phi = Math::mod( phi + 0.04f, 2.0f * Math::PI );
    ratio = ( 1.0f + Math::sin( phi ) ) / 2.0f;

    alpha2 = ratio * ALPHA;
    alpha1 = ( alpha2 * ( 1 - ratio ) ) / ( ratio * ( 1 - alpha2 ) );
  }

  void Water::draw( const Object *obj, bool isInside )
  {
    assert( obj->flags & Object::WATER_BIT );

    float x0 = obj->p.x - obj->dim.x;
    float x1 = obj->p.x + obj->dim.x;
    float y0 = obj->p.y - obj->dim.y;
    float y1 = obj->p.y + obj->dim.y;
    float z1 = obj->p.z + obj->dim.z;

    float s0 = x0 / TEX_STRETCH;
    float s1 = x1 / TEX_STRETCH;
    float t0 = y0 / TEX_STRETCH;
    float t1 = y1 / TEX_STRETCH;

    float u0 = x0 / TEX_STRETCH + TEX_BIAS;
    float u1 = x1 / TEX_STRETCH + TEX_BIAS;
    float v0 = y0 / TEX_STRETCH + TEX_BIAS;
    float v1 = y1 / TEX_STRETCH + TEX_BIAS;

    glBindTexture( GL_TEXTURE_2D, texture );

    if( isInside ) {
      glColor4f( 1.0f, 1.0f, 1.0f, alpha1 );

      glBegin( GL_QUADS );
        glNormal3f( 0.0f, 0.0f, -1.0f );
        glTexCoord2f( s0, t0 );
        glVertex3f( x1, y0, z1 );
        glTexCoord2f( s1, t0 );
        glVertex3f( x0, y0, z1 );
        glTexCoord2f( s1, t1 );
        glVertex3f( x0, y1, z1 );
        glTexCoord2f( s0, t1 );
        glVertex3f( x1, y1, z1 );
      glEnd();

      glColor4f( 1.0f, 1.0f, 1.0f, alpha2 );

      glBegin( GL_QUADS );
      glNormal3f( 0.0f, 0.0f, -1.0f );
      glTexCoord2f( u0, v0 );
      glVertex3f( x1, y0, z1 );
      glTexCoord2f( u1, v0 );
      glVertex3f( x0, y0, z1 );
      glTexCoord2f( u1, v1 );
      glVertex3f( x0, y1, z1 );
      glTexCoord2f( u0, v1 );
      glVertex3f( x1, y1, z1 );
      glEnd();

      glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    }
    else {
      glColor4f( 1.0f, 1.0f, 1.0f, alpha1 );

      glBegin( GL_QUADS );
        glNormal3f( 0.0f, 0.0f, 1.0f );
        glTexCoord2f( s0, t0 );
        glVertex3f( x0, y0, z1 );
        glTexCoord2f( s1, t0 );
        glVertex3f( x1, y0, z1 );
        glTexCoord2f( s1, t1 );
        glVertex3f( x1, y1, z1 );
        glTexCoord2f( s0, t1 );
        glVertex3f( x0, y1, z1 );
      glEnd();

      glColor4f( 1.0f, 1.0f, 1.0f, alpha2 );

      glBegin( GL_QUADS );
        glNormal3f( 0.0f, 0.0f, 1.0f );
        glTexCoord2f( u0, v0 );
        glVertex3f( x0, y0, z1 );
        glTexCoord2f( u1, v0 );
        glVertex3f( x1, y0, z1 );
        glTexCoord2f( u1, v1 );
        glVertex3f( x1, y1, z1 );
        glTexCoord2f( u0, v1 );
        glVertex3f( x0, y1, z1 );
      glEnd();

      glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    }
  }

}
}
