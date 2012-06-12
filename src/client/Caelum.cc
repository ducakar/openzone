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
#include "client/Shape.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{

Caelum caelum;

const Vec4  Caelum::GLOBAL_AMBIENT_COLOUR = Vec4( 0.20f, 0.20f, 0.25f, 1.00f );

const float Caelum::DAY_BIAS              = 0.40f;
const float Caelum::AMBIENT_COEF          = 0.60f;

const float Caelum::RED_COEF              = +0.05f;
const float Caelum::GREEN_COEF            = -0.05f;
const float Caelum::BLUE_COEF             = -0.10f;

const Vec4  Caelum::DAY_COLOUR            = Vec4( 0.45f, 0.60f, 0.90f, 1.0f );
const Vec4  Caelum::NIGHT_COLOUR          = Vec4( 0.02f, 0.02f, 0.05f, 1.0f );
const Vec4  Caelum::STAR_COLOUR           = Vec4( 0.80f, 0.80f, 0.80f, 1.0f );

Caelum::Caelum() :
  vbo( 0 ), ibo( 0 ), sunTexId( 0 ), moonTexId( 0 ), lightDir( 0.0f, 0.0f, 1.0f ),
  diffuseColour( 1.0f, 1.0f, 1.0f, 1.0f ), ambientColour( 1.0f, 1.0f, 1.0f, 1.0f ),
  caelumColour( 1.0f, 1.0f, 1.0f, 1.0f ), id( -1 )
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

  caelumColour.x = Math::mix( NIGHT_COLOUR.x, DAY_COLOUR.x, ratio ) + RED_COEF   * ratioDiff;
  caelumColour.y = Math::mix( NIGHT_COLOUR.y, DAY_COLOUR.y, ratio ) + GREEN_COEF * ratioDiff;
  caelumColour.z = Math::mix( NIGHT_COLOUR.z, DAY_COLOUR.z, ratio ) + BLUE_COEF  * ratioDiff;

  if( camera.nightVision ) {
    caelumColour.x = 0.0f;
    caelumColour.y = caelumColour.x + caelumColour.y + caelumColour.z;
    caelumColour.z = 0.0f;
  }

  lightDir = dir;

  diffuseColour.x = ratio + RED_COEF   * ratioDiff;
  diffuseColour.y = ratio + GREEN_COEF * ratioDiff;
  diffuseColour.z = ratio + BLUE_COEF  * ratioDiff;

  ambientColour.x = AMBIENT_COEF * diffuseColour.x;
  ambientColour.y = AMBIENT_COEF * diffuseColour.y;
  ambientColour.z = AMBIENT_COEF * diffuseColour.z;
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

    glUniform4fv( param.oz_Fog_colour, 1, caelum.caelumColour );
    glUniform4fv( param.oz_Colour, 1, colour );
    glUniform1i( param.oz_NightVision, camera.nightVision );

    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );

    glEnableVertexAttribArray( Attrib::POSITION );
    glVertexAttribPointer( Attrib::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( float[3] ), null );

    glDrawElements( GL_TRIANGLE_STRIP, MAX_STARS * 6, GL_UNSIGNED_SHORT, null );
  }

  shape.bind();

  shader.use( celestialShaderId );
  tf.applyCamera();

  glEnable( GL_BLEND );

  glUniform4f( param.oz_Colour,
               caelum.ambientColour.x + 2.0f * caelum.diffuseColour.x,
               caelum.ambientColour.y + caelum.diffuseColour.y,
               caelum.ambientColour.z + caelum.diffuseColour.z,
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

  Log::print( "Loading Caelum '%s' ...", name.cstr() );

  axis = Vec3( -Math::sin( orbis.caelum.heading ), Math::cos( orbis.caelum.heading ), 0.0f );
  originalLightDir = Vec3( Math::cos( orbis.caelum.heading ),
                           Math::sin( orbis.caelum.heading ),
                           0.0f );

  PFile file( path );
  if( !file.map() ) {
    throw Exception( "Caelum file '%s' mmap failed", path.cstr() );
  }

  InputStream is = file.inputStream();

  int vboSize = MAX_STARS * 4 * int( sizeof( float[3] ) );
  int iboSize = MAX_STARS * 6 * int( sizeof( ushort ) );

  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, vboSize, is.forward( vboSize ), GL_STATIC_DRAW );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );

  glGenBuffers( 1, &ibo );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, iboSize, is.forward( iboSize ), GL_STATIC_DRAW );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

  sunTexId  = context.readTextureLayer( &is, path );
  moonTexId = context.readTextureLayer( &is, path );

  starShaderId      = library.shaderIndex( "stars" );
  celestialShaderId = library.shaderIndex( "celestial" );

  nightColour = Vec4( NIGHT_COLOUR );
  nightLuminance = ( nightColour.x + nightColour.y + nightColour.z ) / 3.0f;

  file.unmap();

  OZ_GL_CHECK_ERROR();

  update();

  Log::printEnd( " OK" );
}

void Caelum::unload()
{
  if( id != -1 ) {
    glDeleteTextures( 1, &sunTexId );
    glDeleteTextures( 1, &moonTexId );

    glDeleteBuffers( 1, &ibo );
    glDeleteBuffers( 1, &vbo );

    sunTexId = 0;
    moonTexId = 0;

    ibo = 0;
    vbo = 0;

    lightDir      = Vec3( 0.0f, 0.0f, 1.0f );

    diffuseColour = Vec4::ONE;
    ambientColour = Vec4::ONE;
    caelumColour  = Vec4::ONE;

    id = -1;
  }
}

}
}
