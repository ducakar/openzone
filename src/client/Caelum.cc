/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/Caelum.cc
 */

#include "stable.hh"

#include "client/Caelum.hh"

#include "client/Context.hh"
#include "client/Camera.hh"
#include "client/Terra.hh"
#include "client/Colours.hh"
#include "client/Shape.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{

Caelum caelum;

const float Caelum::DAY_BIAS       = 0.40f;
const float Caelum::AMBIENT_COEF   = 0.60f;

const float Caelum::RED_COEF       = +0.05f;
const float Caelum::GREEN_COEF     = -0.05f;
const float Caelum::BLUE_COEF      = -0.10f;

const float Caelum::DAY_COLOUR[]   = { 0.45f, 0.60f, 0.90f, 1.0f };
const float Caelum::NIGHT_COLOUR[] = { 0.02f, 0.02f, 0.05f, 1.0f };
const float Caelum::STAR_COLOUR[]  = { 0.80f, 0.80f, 0.80f, 1.0f };

Caelum::Caelum() :
  vao( 0 ), vbo( 0 ), sunTexId( 0 ), moonTexId( 0 ), lightDir( Vec3( 0.0f, 0.0f, 1.0f ) ), id( -1 )
{}

void Caelum::update()
{
  if( id == -1 ) {
    return;
  }

  angle = Math::TAU * ( orbis.caelum.time / orbis.caelum.period );

  Mat44 rot = Mat44::rotation( Quat::rotAxis( axis, angle ) );
  Vec3  dir = rot * originalLightDir;

  ratio = clamp( -dir.z + DAY_BIAS, 0.0f, 1.0f );
  float ratioDiff = ( 1.0f - Math::fabs( 1.0f - 2.0f * ratio ) );

  Colours::caelum.x = Math::mix( NIGHT_COLOUR[0], DAY_COLOUR[0], ratio ) + RED_COEF   * ratioDiff;
  Colours::caelum.y = Math::mix( NIGHT_COLOUR[1], DAY_COLOUR[1], ratio ) + GREEN_COEF * ratioDiff;
  Colours::caelum.z = Math::mix( NIGHT_COLOUR[2], DAY_COLOUR[2], ratio ) + BLUE_COEF  * ratioDiff;

  Colours::liquid.x = Math::mix( NIGHT_COLOUR[0], terra.liquidFogColour.x, ratio );
  Colours::liquid.y = Math::mix( NIGHT_COLOUR[1], terra.liquidFogColour.y, ratio );
  Colours::liquid.z = Math::mix( NIGHT_COLOUR[2], terra.liquidFogColour.z, ratio );

  if( camera.nightVision ) {
    Colours::caelum.x = 0.0f;
    Colours::caelum.y = Colours::caelum.x + Colours::caelum.y + Colours::caelum.z;
    Colours::caelum.z = 0.0f;

    Colours::liquid.x = 0.0f;
    Colours::liquid.y = Colours::liquid.x + Colours::liquid.y + Colours::liquid.z;
    Colours::liquid.z = 0.0f;
  }

  lightDir = dir;

  Colours::diffuse[0] = ratio + RED_COEF   * ratioDiff;
  Colours::diffuse[1] = ratio + GREEN_COEF * ratioDiff;
  Colours::diffuse[2] = ratio + BLUE_COEF  * ratioDiff;

  Colours::ambient[0] = AMBIENT_COEF * Colours::diffuse[0];
  Colours::ambient[1] = AMBIENT_COEF * Colours::diffuse[1];
  Colours::ambient[2] = AMBIENT_COEF * Colours::diffuse[2];
}

void Caelum::draw()
{
  if( id == -1 ) {
    return;
  }

  OZ_GL_CHECK_ERROR();

  // we need the transformation matrix for occlusion of stars below horizon
  Mat44 transf = Mat44::rotationZ( orbis.caelum.heading ) *
  Mat44::rotationY( angle - Math::TAU / 4.0f );

  if( !shader.isLowDetail ) {
    float colour[4] = {
      Math::mix( STAR_COLOUR[0], DAY_COLOUR[0], ratio ),
      Math::mix( STAR_COLOUR[1], DAY_COLOUR[1], ratio ),
      Math::mix( STAR_COLOUR[2], DAY_COLOUR[2], ratio ),
      1.0f
    };

    shader.use( starShaderId );
    tf.applyCamera();

    tf.model = transf;
    tf.apply();

    glUniform4fv( param.oz_Fog_colour, 1, Colours::caelum );
    glUniform4fv( param.oz_Colour, 1, colour );
    glUniform1i( param.oz_NightVision, camera.nightVision );

#ifdef OZ_GL_COMPATIBLE
    glBindBuffer( GL_ARRAY_BUFFER, vbo );

    glEnableVertexAttribArray( Attrib::POSITION );
    glVertexAttribPointer( Attrib::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( float[3] ), null );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
#else
    glBindVertexArray( vao );
#endif

    glDrawElements( GL_TRIANGLE_STRIP, MAX_STARS * 6, GL_UNSIGNED_SHORT, null );
  }

  shape.bindVertexArray();

  shader.use( celestialShaderId );
  tf.applyCamera();

  glEnable( GL_BLEND );

  glUniform4f( param.oz_Colour,
               2.0f * Colours::diffuse[0] + Colours::ambient[0],
               Colours::diffuse[1] + Colours::ambient[1],
               Colours::diffuse[2] + Colours::ambient[2],
               1.0f );
  glUniform1i( param.oz_NightVision, camera.nightVision );
  glBindTexture( GL_TEXTURE_2D, sunTexId );

  glDisable( GL_CULL_FACE );

  tf.model = transf;
  tf.model.translate( Vec3( 0.0f, 0.0f, +15.0f ) );

  shape.quad( 1.0f, 1.0f );

  glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );
  glBindTexture( GL_TEXTURE_2D, moonTexId );

  tf.model = transf;
  tf.model.translate( Vec3( 0.0f, 0.0f, -15.0f ) );

  shape.quad( 1.0f, 1.0f );

  glDisable( GL_BLEND );

  glEnable( GL_CULL_FACE );

  OZ_GL_CHECK_ERROR();
}

void Caelum::load()
{
  id = orbis.caelum.id;

  const String& name = library.caela[id].name;
  const String& path = library.caela[id].path;

  log.print( "Loading Caelum '%s' ...", name.cstr() );

  axis = Vec3( -Math::sin( orbis.caelum.heading ), Math::cos( orbis.caelum.heading ), 0.0f );
  originalLightDir = Vec3( Math::cos( orbis.caelum.heading ),
                           Math::sin( orbis.caelum.heading ),
                           0.0f );

  PhysFile file( path );
  if( !file.map() ) {
    throw Exception( "Caelum file '%s' mmap failed", path.cstr() );
  }

  InputStream is = file.inputStream();

  int vboSize = MAX_STARS * 4 * int( sizeof( float[3] ) );
  int iboSize = MAX_STARS * 6 * int( sizeof( ushort ) );

#ifndef OZ_GL_COMPATIBLE
  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );
#endif

  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, vboSize, is.forward( vboSize ), GL_STATIC_DRAW );

  glGenBuffers( 1, &ibo );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, iboSize, is.forward( iboSize ), GL_STATIC_DRAW );

#ifndef OZ_GL_COMPATIBLE
  glEnableVertexAttribArray( Attrib::POSITION );
  glVertexAttribPointer( Attrib::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( float[3] ), null );

  glBindVertexArray( 0 );
#endif

  glBindBuffer( GL_ARRAY_BUFFER, 0 );

  sunTexId  = context.readTextureLayer( &is, path );
  moonTexId = context.readTextureLayer( &is, path );

  starShaderId      = library.shaderIndex( "stars" );
  celestialShaderId = library.shaderIndex( "celestial" );

//   nightColour = is.readVec4();
  nightColour = Vec4( NIGHT_COLOUR );
  nightLuminance = ( nightColour.x + nightColour.y + nightColour.z ) / 3.0f;

  file.unmap();

  OZ_GL_CHECK_ERROR();

  update();

  log.printEnd( " OK" );
}

void Caelum::unload()
{
  if( id != -1 ) {
    glDeleteTextures( 1, &sunTexId );
    glDeleteTextures( 1, &moonTexId );

    glDeleteBuffers( 1, &vbo );
#ifndef OZ_GL_COMPATIBLE
    glDeleteVertexArrays( 1, &vao );
#endif

    sunTexId = 0;
    moonTexId = 0;
    vbo = 0;
    vao = 0;

    Colours::diffuse = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
    Colours::ambient = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
    Colours::caelum  = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
    Colours::liquid  = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );

    lightDir = Vec3( 0.0f, 0.0f, 1.0f );

    id = -1;
  }
}

}
}
