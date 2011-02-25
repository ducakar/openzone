/*
 *  Sky.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Sky.hpp"

#include "matrix/Timer.hpp"
#include "matrix/Orbis.hpp"
#include "client/Context.hpp"
#include "client/Camera.hpp"
#include "client/Colours.hpp"

namespace oz
{
namespace client
{

  Sky sky;

  const float Sky::DAY_BIAS = 0.4f;

  const float Sky::AMBIENT_COEF = 0.40f;

  const float Sky::RED_COEF   = +0.05f;
  const float Sky::GREEN_COEF = -0.05f;
  const float Sky::BLUE_COEF  = -0.10f;

  const float Sky::DAY_COLOUR[]   = { 0.45f, 0.60f, 0.90f, 1.0f };
  const float Sky::NIGHT_COLOUR[] = { 0.02f, 0.02f, 0.05f, 1.0f };
  const float Sky::WATER_COLOUR[] = { 0.00f, 0.05f, 0.25f, 1.0f };
  const float Sky::STAR_COLOUR[]  = { 0.80f, 0.80f, 0.80f, 1.0f };

  struct StarEntry
  {
    Point3 p;
    float  coef;

    bool operator < ( const StarEntry& se )
    {
      return coef < se.coef;
    }
  };

  void Sky::load()
  {
    float heading = Math::rad( orbis.sky.heading );

    axis = Vec3( -Math::sin( heading ), Math::cos( heading ), 0.0f );
    originalLightDir = Vec3( -Math::cos( heading ), -Math::sin( heading ), 0.0f );

    StarEntry* tempStars = new StarEntry[MAX_STARS];
    for( int i = 0; i < MAX_STARS; ++i ) {
      float length;
      do {
        tempStars[i].p.x  = 20.0f * Math::frand() - 10.0f;
        tempStars[i].p.y  = 20.0f * Math::frand() - 10.0f;
        tempStars[i].p.z  = 20.0f * Math::frand() - 10.0f;
        tempStars[i].coef = Math::atan2( tempStars[i].p.z, tempStars[i].p.x );
        length = Vec3( tempStars[i].p ).sqL();
      }
      while( Math::isNaN( length ) || length < 1.0f || length > 100.0f );
    }

    // sort stars
    aSort( tempStars, MAX_STARS );

    for( int i = 0; i < MAX_STARS; ++i ) {
      stars[i] = tempStars[i].p;
    }

    delete[] tempStars;

    Vertex* vertices = new Vertex[MAX_STARS];
    for( int i = 0; i < MAX_STARS; ++i ) {
      vertices[i].set( stars[i] );
    }

//     starArray = context.genArray( GL_STATIC_DRAW, vertices, MAX_STARS );
    delete[] vertices;

    sunTexId  = context.loadRawTexture( "sky/simplesun.png", false, GL_LINEAR, GL_LINEAR );
    moonTexId = context.loadRawTexture( "sky/moon18.png", false, GL_LINEAR, GL_LINEAR );

    sunList  = context.genLists( 2 );
    moonList = sunList + 1;

    glNewList( sunList, GL_COMPILE );

    glBindTexture( GL_TEXTURE_2D, sunTexId );
    glBegin( GL_QUADS );
      glTexCoord2f( 0.0f, 1.0f );
      glVertex3f( 0.0f, -1.0f, +1.0f );
      glTexCoord2f( 1.0f, 1.0f );
      glVertex3f( 0.0f, -1.0f, -1.0f );
      glTexCoord2f( 1.0f, 0.0f );
      glVertex3f( 0.0f, +1.0f, -1.0f );
      glTexCoord2f( 0.0f, 0.0f );
      glVertex3f( 0.0f, +1.0f, +1.0f );
    glEnd();

    glEndList();

    glNewList( moonList, GL_COMPILE );

    glColor3f( 1.0f, 1.0f, 1.0f );
    glBindTexture( GL_TEXTURE_2D, moonTexId );
    glBegin( GL_QUADS );
      glTexCoord2f( 0.0f, 1.0f );
      glVertex3f( 0.0f, -1.0f, -1.0f );
      glTexCoord2f( 1.0f, 1.0f );
      glVertex3f( 0.0f, -1.0f, +1.0f );
      glTexCoord2f( 1.0f, 0.0f );
      glVertex3f( 0.0f, +1.0f, +1.0f );
      glTexCoord2f( 0.0f, 0.0f );
      glVertex3f( 0.0f, +1.0f, -1.0f );
    glEnd();

    glEndList();

    update();
  }

  void Sky::unload()
  {
    context.deleteLists( sunList );
    glDeleteTextures( 1, &sunTexId );
    glDeleteTextures( 1, &moonTexId );
//     context.deleteArray( starArray );
  }

  void Sky::update()
  {
    angle = 2.0f * Math::PI * ( orbis.sky.time / orbis.sky.period );

    Mat44 rot = Quat::rotAxis( axis, angle ).rotMat44();
    Vec3  dir = rot * originalLightDir;

    ratio = Math::bound( dir.z + DAY_BIAS, 0.0f, 1.0f );
    float ratioDiff = ( 1.0f - Math::abs( 1.0f - 2.0f * ratio ) );

    Colours::sky[0] = NIGHT_COLOUR[0] + ratio * ( DAY_COLOUR[0] - NIGHT_COLOUR[0] ) + RED_COEF * ratioDiff;
    Colours::sky[1] = NIGHT_COLOUR[1] + ratio * ( DAY_COLOUR[1] - NIGHT_COLOUR[1] ) + GREEN_COEF * ratioDiff;
    Colours::sky[2] = NIGHT_COLOUR[2] + ratio * ( DAY_COLOUR[2] - NIGHT_COLOUR[2] ) + BLUE_COEF * ratioDiff;
    Colours::sky[3] = 1.0f;

    Colours::water[0] = NIGHT_COLOUR[0] + ratio * ( WATER_COLOUR[0] - NIGHT_COLOUR[0] );
    Colours::water[1] = NIGHT_COLOUR[1] + ratio * ( WATER_COLOUR[1] - NIGHT_COLOUR[1] );
    Colours::water[2] = NIGHT_COLOUR[2] + ratio * ( WATER_COLOUR[2] - NIGHT_COLOUR[2] );
    Colours::water[3] = NIGHT_COLOUR[3] + ratio * ( WATER_COLOUR[3] - NIGHT_COLOUR[3] );

    lightDir[0] = dir.x;
    lightDir[1] = dir.y;
    lightDir[2] = dir.z;
    lightDir[3] = 0.0f;

    Colours::diffuse[0] = ratio + RED_COEF * ratioDiff;
    Colours::diffuse[1] = ratio + GREEN_COEF * ratioDiff;
    Colours::diffuse[2] = ratio + BLUE_COEF * ratioDiff;
    Colours::diffuse[3] = 1.0f;

    Colours::ambient[0] = AMBIENT_COEF * Colours::diffuse[0];
    Colours::ambient[1] = AMBIENT_COEF * Colours::diffuse[1];
    Colours::ambient[2] = AMBIENT_COEF * Colours::diffuse[2];
    Colours::ambient[3] = 1.0f;
  }

  void Sky::draw()
  {
    float colour[3] = {
      STAR_COLOUR[0] + ratio * ( DAY_COLOUR[0] - STAR_COLOUR[0] ),
      STAR_COLOUR[1] + ratio * ( DAY_COLOUR[1] - STAR_COLOUR[1] ),
      STAR_COLOUR[2] + ratio * ( DAY_COLOUR[2] - STAR_COLOUR[2] )
    };

    // we need the transformation matrix for occlusion of stars below horizon
    Mat44 transf = Mat44::rotZ( Math::rad( orbis.sky.heading ) ) * Mat44::rotY( angle );

    glDisable( GL_BLEND );
    glColor3fv( colour );

    glPushMatrix();
    glMultMatrixf( transf );
    transf = ~transf;

//     context.bindArray( starArray );

//     Vec3 tz = transf.z;
//     int start, end;
//     if( stars[0] * tz > 0.0f ) {
//       for( end = 1; end < MAX_STARS && stars[end] * tz > 0.0f; ++end );
//       for( start = end + 1; start < MAX_STARS && stars[start] * tz <= 0.0f; ++start );
//
//       glDrawArrays( GL_POINTS, 0, end );
//       glDrawArrays( GL_POINTS, start, MAX_STARS - start );
//     }
//     else {
//       for( start = 1; start < MAX_STARS && stars[start] * tz <= 0.0f; ++start );
//       for( end = start; end < MAX_STARS && stars[end] * tz > 0.0f; ++end );
//
//       glDrawArrays( GL_POINTS, start, end - start );
//     }

    glEnable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );

    glTranslatef( -15.0f, 0.0f, 0.0f );
    glColor3f( 2.0f * Colours::diffuse[0] + Colours::ambient[0],
               Colours::diffuse[1] + Colours::ambient[1],
               Colours::diffuse[2] + Colours::ambient[2] );
    glCallList( sunList );

    glTranslatef( 30.0f, 0.0f, 0.0f );
    glCallList( moonList );

    glPopMatrix();

    hard_assert( glGetError() == GL_NO_ERROR );
  }

}
}
