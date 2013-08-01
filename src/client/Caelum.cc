/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <client/Caelum.hh>

#include <client/Shader.hh>
#include <client/Shape.hh>

namespace oz
{
namespace client
{

const Vec4  Caelum::GLOBAL_AMBIENT_COLOUR = Vec4( 0.12f, 0.12f, 0.15f, 1.00f );

const float Caelum::DAY_BIAS              = 0.40f;
const float Caelum::AMBIENT_COEF          = 0.60f;

const float Caelum::RED_COEF              = +0.05f;
const float Caelum::GREEN_COEF            = -0.05f;
const float Caelum::BLUE_COEF             = -0.10f;

const Vec4  Caelum::DAY_COLOUR            = Vec4( 0.45f, 0.60f, 0.90f, 1.0f );
const Vec4  Caelum::NIGHT_COLOUR          = Vec4( 0.02f, 0.02f, 0.05f, 1.0f );
const Vec4  Caelum::STARS_COLOUR          = Vec4( 0.80f, 0.80f, 0.80f, 1.0f );

Caelum::Caelum() :
  vbo( 0 ), ibo( 0 ), sunTexId( 0 ), moonTexId( 0 ), lightDir( 0.0f, 0.0f, 1.0f ),
  diffuseColour( 1.0f, 1.0f, 1.0f, 1.0f ), ambientColour( 1.0f, 1.0f, 1.0f, 1.0f ),
  caelumColour( 1.0f, 1.0f, 1.0f, 1.0f ), id( -1 )
{}

void Caelum::update()
{
  if( id < 0 ) {
    return;
  }

  angle = Math::TAU * ( orbis.caelum.time / orbis.caelum.period );

  Mat44 rot = Mat44::rotation( Quat::rotationAxis( axis, angle ) );
  Vec3  dir = rot * originalLightDir;

  ratio = clamp( dir.z + DAY_BIAS, 0.0f, 1.0f );
  float ratioDiff = 1.0f - abs( 1.0f - 2.0f * ratio );

  caelumColour.x = Math::mix( NIGHT_COLOUR.x, DAY_COLOUR.x, ratio ) + RED_COEF   * ratioDiff;
  caelumColour.y = Math::mix( NIGHT_COLOUR.y, DAY_COLOUR.y, ratio ) + GREEN_COEF * ratioDiff;
  caelumColour.z = Math::mix( NIGHT_COLOUR.z, DAY_COLOUR.z, ratio ) + BLUE_COEF  * ratioDiff;

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
  if( id < 0 ) {
    return;
  }

  OZ_GL_CHECK_ERROR();

  // we need the transformation matrix for occlusion of stars below horizon
  Mat44 transf = Mat44::rotationZ( orbis.caelum.heading ) *
  Mat44::rotationY( angle - Math::TAU / 4.0f );

  if( !shader.isLowDetail ) {
    Vec4 starsColour = Math::mix( STARS_COLOUR, caelumColour, ratio );

    shader.program( starShaderId );

    tf.model = transf;
    tf.applyCamera();
    tf.apply();
    tf.applyColour();

    glUniform4fv( uniform.fog_colour, 1, caelumColour );
    glUniform4fv( uniform.starsColour, 1, starsColour );

    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );

    glEnableVertexAttribArray( Attrib::POSITION );
    glVertexAttribPointer( Attrib::POSITION, 3, GL_FLOAT, GL_FALSE, int( sizeof( float[3] ) ),
                           nullptr );

    glDrawElements( GL_TRIANGLE_STRIP, MAX_STARS * 6, GL_UNSIGNED_SHORT, nullptr );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
  }

  shape.bind();
  shader.program( celestialShaderId );

  Vec4 sunColour = Vec4( ambientColour.x + 2.0f * diffuseColour.x,
                         ambientColour.y + diffuseColour.y,
                         ambientColour.z + diffuseColour.z,
                         1.0f );

  tf.applyCamera();
  tf.apply();
  tf.setColour( tf.colour * sunColour );

  glDisable( GL_CULL_FACE );
  glBindTexture( GL_TEXTURE_2D, sunTexId );

  tf.model = transf;
  tf.model.translate( Vec3( 0.0f, 0.0f, +15.0f ) );

  shape.quad( 1.0f, 1.0f );

  glBindTexture( GL_TEXTURE_2D, moonTexId );

  tf.model = transf;
  tf.model.translate( Vec3( 0.0f, 0.0f, -15.0f ) );
  tf.applyColour();

  shape.quad( 1.0f, 1.0f );

  glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
  glEnable( GL_CULL_FACE );

  shape.unbind();

  OZ_GL_CHECK_ERROR();
}

void Caelum::load()
{
  id = orbis.caelum.id;

  File file = liber.caela[id].path;
  InputStream is = file.inputStream( Endian::LITTLE );

  if( !is.isAvailable() ) {
    OZ_ERROR( "Caelum file '%s' read failed", file.path().cstr() );
  }

  glGenTextures( 1, &sunTexId );
  glBindTexture( GL_TEXTURE_2D, sunTexId );

  if( !GL::textureDataFromFile( "@caelum/sun.dds" ) ) {
    OZ_ERROR( "Failed to load sun texture" );
  }

  glGenTextures( 1, &moonTexId );
  glBindTexture( GL_TEXTURE_2D, moonTexId );

  if( !GL::textureDataFromFile( "@caelum/moon.dds" ) ) {
    OZ_ERROR( "Failed to load moon texture" );
  }

  glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );

  axis = Vec3( -Math::sin( orbis.caelum.heading ), Math::cos( orbis.caelum.heading ), 0.0f );
  originalLightDir = Vec3( -Math::cos( orbis.caelum.heading ),
                           -Math::sin( orbis.caelum.heading ),
                           0.0f );

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

  starShaderId      = liber.shaderIndex( "stars" );
  celestialShaderId = liber.shaderIndex( "celestial" );

  nightColour = Vec4( NIGHT_COLOUR );
  nightLuminance = ( nightColour.x + nightColour.y + nightColour.z ) / 3.0f;

  OZ_GL_CHECK_ERROR();

  update();
}

void Caelum::unload()
{
  if( id >= 0 ) {
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

Caelum caelum;

}
}
