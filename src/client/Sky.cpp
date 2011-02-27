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
#include "client/Shape.hpp"

namespace oz
{
namespace client
{

  Sky sky;

  const float Sky::DAY_BIAS       = 0.4f;

  const float Sky::AMBIENT_COEF   = 0.40f;

  const float Sky::RED_COEF       = +0.05f;
  const float Sky::GREEN_COEF     = -0.05f;
  const float Sky::BLUE_COEF      = -0.10f;

  const float Sky::DAY_COLOUR[]   = { 0.45f, 0.60f, 0.90f, 1.0f };
  const float Sky::NIGHT_COLOUR[] = { 0.02f, 0.02f, 0.05f, 1.0f };
  const float Sky::WATER_COLOUR[] = { 0.00f, 0.05f, 0.25f, 1.0f };
  const float Sky::STAR_COLOUR[]  = { 0.80f, 0.80f, 0.80f, 1.0f };

  const float Sky::STAR_DIM       = 0.10f;

  void Sky::load()
  {
    float heading = Math::rad( orbis.sky.heading );

    axis = Vec3( -Math::sin( heading ), Math::cos( heading ), 0.0f );
    originalLightDir = Vec3( -Math::cos( heading ), -Math::sin( heading ), 0.0f );

    DArray<Point3> positions( MAX_STARS );
    DArray<ushort> indices( MAX_STARS * 18 );
    DArray<Vertex> vertices( MAX_STARS * 6 );

    for( int i = 0; i < MAX_STARS; ++i ) {
      float length;
      do {
        positions[i] = Point3( 200.0f * Math::frand() - 100.0f,
                               200.0f * Math::frand() - 100.0f,
                               200.0f * Math::frand() - 100.0f );
        length = ( positions[i] - Point3::ORIGIN ).sqL();
      }
      while( Math::isNaN( length ) || length < 1600.0f || length > 10000.0f );
    }

    ushort* index     = indices;
    Vertex* vertex    = vertices;
    int     vertIndex = 0;

    for( int i = 0; i < MAX_STARS; ++i ) {
      index[ 0] = ushort( vertIndex + 0 );
      index[ 1] = ushort( vertIndex + 0 );
      index[ 2] = ushort( vertIndex + 1 );
      index[ 3] = ushort( vertIndex + 4 );
      index[ 4] = ushort( vertIndex + 5 );
      index[ 5] = ushort( vertIndex + 5 );

      index[ 6] = ushort( vertIndex + 0 );
      index[ 7] = ushort( vertIndex + 0 );
      index[ 8] = ushort( vertIndex + 2 );
      index[ 9] = ushort( vertIndex + 3 );
      index[10] = ushort( vertIndex + 5 );
      index[11] = ushort( vertIndex + 5 );

      index[12] = ushort( vertIndex + 1 );
      index[13] = ushort( vertIndex + 1 );
      index[14] = ushort( vertIndex + 2 );
      index[15] = ushort( vertIndex + 3 );
      index[16] = ushort( vertIndex + 4 );
      index[17] = ushort( vertIndex + 4 );

      index += 18;
      vertIndex += 6;
    }

    for( int i = 0; i < MAX_STARS; ++i ) {
      vertex[0].set( positions[i] + Vec3( -STAR_DIM, 0.0f, 0.0f ) );
      vertex[1].set( positions[i] + Vec3( 0.0f, -STAR_DIM, 0.0f ) );
      vertex[2].set( positions[i] + Vec3( 0.0f, 0.0f, -STAR_DIM ) );
      vertex[3].set( positions[i] + Vec3( 0.0f, 0.0f, +STAR_DIM ) );
      vertex[4].set( positions[i] + Vec3( 0.0f, +STAR_DIM, 0.0f ) );
      vertex[5].set( positions[i] + Vec3( +STAR_DIM, 0.0f, 0.0f ) );

      vertex += 6;
    }

    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    glGenBuffers( 1, &ibo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, ( indices.length() - 2 ) * sizeof( ushort ), indices,
                  GL_STATIC_DRAW );

    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, vertices.length() * sizeof( Vertex ), vertices,
                  GL_STATIC_DRAW );

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, sizeof( Vertex ),
                     reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, pos ) );

    glBindVertexArray( 0 );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    hard_assert( glGetError() == GL_NO_ERROR );

    sunTexId  = context.loadRawTexture( "sky/simplesun.png", null, false, GL_LINEAR, GL_LINEAR );
    moonTexId = context.loadRawTexture( "sky/moon18.png", null, false, GL_LINEAR, GL_LINEAR );

    update();
  }

  void Sky::unload()
  {
    glDeleteTextures( 1, &sunTexId );
    glDeleteTextures( 1, &moonTexId );

    glDeleteVertexArrays( 1, &vao );
    glDeleteBuffers( 1, &vbo );
  }

  void Sky::update()
  {
    angle = 2.0f * Math::PI * ( orbis.sky.time / orbis.sky.period );

    Mat44 rot = Quat::rotAxis( axis, angle ).rotMat44();
    Vec3  dir = rot * originalLightDir;

    ratio = bound( dir.z + DAY_BIAS, 0.0f, 1.0f );
    float ratioDiff = ( 1.0f - Math::abs( 1.0f - 2.0f * ratio ) );

    Colours::sky[0] = NIGHT_COLOUR[0] + ratio * ( DAY_COLOUR[0] - NIGHT_COLOUR[0] ) + RED_COEF * ratioDiff;
    Colours::sky[1] = NIGHT_COLOUR[1] + ratio * ( DAY_COLOUR[1] - NIGHT_COLOUR[1] ) + GREEN_COEF * ratioDiff;
    Colours::sky[2] = NIGHT_COLOUR[2] + ratio * ( DAY_COLOUR[2] - NIGHT_COLOUR[2] ) + BLUE_COEF * ratioDiff;
    Colours::sky[3] = 1.0f;

    Colours::water[0] = NIGHT_COLOUR[0] + ratio * ( WATER_COLOUR[0] - NIGHT_COLOUR[0] );
    Colours::water[1] = NIGHT_COLOUR[1] + ratio * ( WATER_COLOUR[1] - NIGHT_COLOUR[1] );
    Colours::water[2] = NIGHT_COLOUR[2] + ratio * ( WATER_COLOUR[2] - NIGHT_COLOUR[2] );
    Colours::water[3] = NIGHT_COLOUR[3] + ratio * ( WATER_COLOUR[3] - NIGHT_COLOUR[3] );

    lightDir = dir;

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
    float colour[4] = {
      STAR_COLOUR[0] + ratio * ( DAY_COLOUR[0] - STAR_COLOUR[0] ),
      STAR_COLOUR[1] + ratio * ( DAY_COLOUR[1] - STAR_COLOUR[1] ),
      STAR_COLOUR[2] + ratio * ( DAY_COLOUR[2] - STAR_COLOUR[2] ),
      1.0f
    };

    // we need the transformation matrix for occlusion of stars below horizon
    Mat44 transf = Mat44::rotZ( Math::rad( orbis.sky.heading ) ) * Mat44::rotY( angle );

    glDisable( GL_CULL_FACE );

    glPushMatrix();
    glMultMatrixf( transf );

    hard_assert( glGetError() == GL_NO_ERROR );

    glBindVertexArray( vao );

    shader.use( Shader::STARS );
    glUniform4f( Param::oz_DiffuseMaterial, colour[0], colour[1], colour[2], colour[3] );
    glDrawElements( GL_TRIANGLE_STRIP, MAX_STARS * 18 - 2, GL_UNSIGNED_SHORT, 0 );

    shape.bindVertexArray();

    shader.use( Shader::UI );
    glEnable( GL_BLEND );

    glColor3f( 2.0f * Colours::diffuse[0] + Colours::ambient[0],
               Colours::diffuse[1] + Colours::ambient[1],
               Colours::diffuse[2] + Colours::ambient[2] );
    shader.bindTextures( sunTexId );

    shape.drawSprite( Point3( -15.0f, 0.0f, 0.0f ), 1.0f, 1.0f );

    glColor4fv( Colours::WHITE );
    shader.bindTextures( moonTexId );

    shape.drawSprite( Point3( 15.0f, 0.0f, 0.0f ), 1.0f, 1.0f );

    glDisable( GL_BLEND );

    glColor4fv( Colours::BLACK );
    shader.use( Shader::DEFAULT );

    glPopMatrix();

    glEnable( GL_CULL_FACE );

    hard_assert( glGetError() == GL_NO_ERROR );
  }

}
}
