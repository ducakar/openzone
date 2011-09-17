/*
 *  Caelum.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Caelum.hpp"

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

  Caelum caelum;

  const float Caelum::DAY_BIAS       = 0.4f;

  const float Caelum::AMBIENT_COEF   = 0.40f;

  const float Caelum::RED_COEF       = +0.05f;
  const float Caelum::GREEN_COEF     = -0.05f;
  const float Caelum::BLUE_COEF      = -0.10f;

  const float Caelum::DAY_COLOUR[]   = { 0.45f, 0.60f, 0.90f, 1.0f };
  const float Caelum::NIGHT_COLOUR[] = { 0.02f, 0.02f, 0.05f, 1.0f };
  const float Caelum::WATER_COLOUR[] = { 0.00f, 0.05f, 0.25f, 1.0f };
  const float Caelum::STAR_COLOUR[]  = { 0.80f, 0.80f, 0.80f, 1.0f };

  const float Caelum::STAR_DIM       = 0.10f;

  Caelum::Caelum() : vao( 0 ), vbo( 0 ), sunTexId( 0 ), moonTexId( 0 )
  {}

#ifdef OZ_SDK
  void Caelum::prebuild( const char* name )
  {
    log.println( "Prebuilding Caelum '%s' {", name );
    log.indent();

    DArray<Point3> positions( MAX_STARS );

    for( int i = 0; i < MAX_STARS; ++i ) {
      float length;
      do {
        positions[i] = Point3( 200.0f * Math::frand() - 100.0f,
                               200.0f * Math::frand() - 100.0f,
                               200.0f * Math::frand() - 100.0f );
        length = ( positions[i] - Point3::ORIGIN ).sqL();
      }
      while( Math::isNaN( length ) || length < 2500.0f || length > 10000.0f );
    }

    OutputStream os = buffer.outputStream();

    for( int i = 0; i < MAX_STARS; ++i ) {
      Vec3 z = ~( Point3::ORIGIN - positions[i] );
      Vec3 x = ~Vec3( z.z, 0.0f, -z.x );
      Vec3 y = z ^ x;

      Mat44 rot = Mat44( x, y, z, Vec3::ZERO );
      Vertex vertex;

      vertex = Vertex( positions[i] + rot * Vec3( -STAR_DIM, 0.0f, 0.0f ) );
      vertex.write( &os );

      vertex = Vertex( positions[i] + rot * Vec3( 0.0f, -STAR_DIM, 0.0f ) );
      vertex.write( &os );

      vertex = Vertex( positions[i] + rot * Vec3( +STAR_DIM, 0.0f, 0.0f ) );
      vertex.write( &os );

      vertex = Vertex( positions[i] + rot * Vec3( 0.0f, +STAR_DIM, 0.0f ) );
      vertex.write( &os );
    }

    uint texId = context.loadRawTexture( "caelum/simplesun.png", false,
                                         Context::DEFAULT_MAG_FILTER, Context::DEFAULT_MAG_FILTER );
    context.writeTexture( texId, &os );
    glDeleteTextures( 1, &texId );

    texId = context.loadRawTexture( "caelum/moon18.png", false,
                                    Context::DEFAULT_MAG_FILTER, Context::DEFAULT_MAG_FILTER );
    context.writeTexture( texId, &os );
    glDeleteTextures( 1, &texId );

    buffer.write( "caelum/" + String( name ) + ".ozcCaelum", os.length() );

    hard_assert( glGetError() == GL_NO_ERROR );

    log.unindent();
    log.println( "}" );
  }
#endif

  void Caelum::update()
  {
    angle = Math::TAU * ( orbis.caelum.time / orbis.caelum.period );

    Mat44 rot = Mat44::rotation( Quat::rotAxis( axis, angle ) );
    Vec3  dir = rot * originalLightDir;

    ratio = clamp( -dir.z + DAY_BIAS, 0.0f, 1.0f );
    float ratioDiff = ( 1.0f - Math::abs( 1.0f - 2.0f * ratio ) );

    Colours::caelum[0] = NIGHT_COLOUR[0] + ratio * ( DAY_COLOUR[0] - NIGHT_COLOUR[0] ) + RED_COEF * ratioDiff;
    Colours::caelum[1] = NIGHT_COLOUR[1] + ratio * ( DAY_COLOUR[1] - NIGHT_COLOUR[1] ) + GREEN_COEF * ratioDiff;
    Colours::caelum[2] = NIGHT_COLOUR[2] + ratio * ( DAY_COLOUR[2] - NIGHT_COLOUR[2] ) + BLUE_COEF * ratioDiff;
    Colours::caelum[3] = 1.0f;

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

  void Caelum::draw()
  {
    float colour[4] = {
      STAR_COLOUR[0] + ratio * ( DAY_COLOUR[0] - STAR_COLOUR[0] ),
      STAR_COLOUR[1] + ratio * ( DAY_COLOUR[1] - STAR_COLOUR[1] ),
      STAR_COLOUR[2] + ratio * ( DAY_COLOUR[2] - STAR_COLOUR[2] ),
      1.0f
    };

    // we need the transformation matrix for occlusion of stars below horizon
    Mat44 transf = Mat44::rotationZ( orbis.caelum.heading ) *
        Mat44::rotationY( angle - Math::TAU / 4.0f );

    hard_assert( glGetError() == GL_NO_ERROR );

    shader.use( starShaderId );
    tf.applyCamera();

    tf.model = transf;
    tf.apply();

    glBindVertexArray( vao );

    glUniform4fv( param.oz_Fog_colour, 1, Colours::caelum );
    glUniform4fv( param.oz_Colour, 1, colour );

    glDrawArrays( GL_QUADS, 0, MAX_STARS * 4 );

    shape.bindVertexArray();

    shader.use( celestialShaderId );
    tf.applyCamera();

    glEnable( GL_BLEND );

    glUniform4f( param.oz_Colour,
                 2.0f * Colours::diffuse[0] + Colours::ambient[0],
                 Colours::diffuse[1] + Colours::ambient[1],
                 Colours::diffuse[2] + Colours::ambient[2],
                 1.0f );
    glBindTexture( GL_TEXTURE_2D, sunTexId );

    glDisable( GL_CULL_FACE );

    tf.model = transf;
    tf.model.translate( Vec3( 0.0f, 0.0f, +15.0f ) );

    shape.quad( 1.0f, 1.0f );

    glUniform4fv( param.oz_Colour, 1, Colours::WHITE );
    glBindTexture( GL_TEXTURE_2D, moonTexId );

    tf.model = transf;
    tf.model.translate( Vec3( 0.0f, 0.0f, -15.0f ) );

    shape.quad( 1.0f, 1.0f );

    glDisable( GL_BLEND );

    glEnable( GL_CULL_FACE );

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void Caelum::load( const char* name )
  {
    log.print( "Loading Caelum '%s' ...", name );

    axis = Vec3( -Math::sin( orbis.caelum.heading ), Math::cos( orbis.caelum.heading ), 0.0f );
    originalLightDir = Vec3( Math::cos( orbis.caelum.heading ),
                             Math::sin( orbis.caelum.heading ),
                             0.0f );

    DArray<Vertex> vertices(  MAX_STARS * 4 );

    if( !buffer.read( "caelum/" + String( name ) + ".ozcCaelum" ) ) {
      log.printEnd( " Cannot open file" );
      throw Exception( "Caelum loading failed" );
    }

    InputStream is = buffer.inputStream();

    for( int i = 0; i < MAX_STARS * 4; ++i ) {
      vertices[i].read( &is );
    }

    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, vertices.length() * int( sizeof( Vertex ) ), vertices,
                  GL_STATIC_DRAW );
    vertices.dealloc();

    glEnableVertexAttribArray( Attrib::POSITION );
    glVertexAttribPointer( Attrib::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                           reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, pos ) );

    glBindVertexArray( 0 );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    sunTexId  = context.readTexture( &is );
    moonTexId = context.readTexture( &is );

    starShaderId      = translator.shaderIndex( "stars" );
    celestialShaderId = translator.shaderIndex( "celestial" );

    hard_assert( glGetError() == GL_NO_ERROR );

    update();

    log.printEnd( " OK" );
  }

  void Caelum::unload()
  {
    if( vao != 0 ) {
      glDeleteTextures( 1, &sunTexId );
      glDeleteTextures( 1, &moonTexId );

      glDeleteBuffers( 1, &vbo );
      glDeleteVertexArrays( 1, &vao );

      sunTexId = 0;
      moonTexId = 0;
      vbo = 0;
      vao = 0;
    }
  }

}
}
