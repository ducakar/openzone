/*
 *  Caelum.cpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "build/Caelum.hpp"

#include "client/Caelum.hpp"
#include "client/OpenGL.hpp"

#include "build/Mesh.hpp"
#include "build/Context.hpp"

namespace oz
{
namespace build
{

const float Caelum::STAR_DIM = 0.10f;

void Caelum::build( const char* name )
{
  log.println( "Prebuilding Caelum '%s' {", name );
  log.indent();

  String destPath = "caelum/" + String( name ) + ".ozcCaelum";

  DArray<Point3> positions( client::Caelum::MAX_STARS );

  for( int i = 0; i < client::Caelum::MAX_STARS; ++i ) {
    float length;
    do {
      positions[i] = Point3( 200.0f * Math::rand() - 100.0f,
                             200.0f * Math::rand() - 100.0f,
                             200.0f * Math::rand() - 100.0f );
      length = ( positions[i] - Point3::ORIGIN ).sqL();
    }
    while( Math::isNaN( length ) || length < 2500.0f || length > 10000.0f );
  }

  Buffer buffer( 4 * 1024 * 1024 );
  OutputStream os = buffer.outputStream();

  for( int i = 0; i < client::Caelum::MAX_STARS; ++i ) {
    Vec3 z = ~( Point3::ORIGIN - positions[i] );
    Vec3 x = ~Vec3( z.z, 0.0f, -z.x );
    Vec3 y = z ^ x;

    Mat44 rot = Mat44( x, y, z, Vec3::ZERO );
    client::Vertex vertex;

    vertex = Vertex( positions[i] + rot * Vec3( -STAR_DIM, 0.0f, 0.0f ) );
    vertex.write( &os );

    vertex = Vertex( positions[i] + rot * Vec3( 0.0f, -STAR_DIM, 0.0f ) );
    vertex.write( &os );

    vertex = Vertex( positions[i] + rot * Vec3( +STAR_DIM, 0.0f, 0.0f ) );
    vertex.write( &os );

    vertex = Vertex( positions[i] + rot * Vec3( 0.0f, +STAR_DIM, 0.0f ) );
    vertex.write( &os );
  }

  uint texId = Context::loadRawTexture( "caelum/sun.png", false );
  Context::writeTexture( texId, &os );
  glDeleteTextures( 1, &texId );

  texId = Context::loadRawTexture( "caelum/moon.png", false );
  Context::writeTexture( texId, &os );
  glDeleteTextures( 1, &texId );

  log.print( "Dumping into '%s' ...", destPath.cstr() );
  buffer.write( destPath, os.length() );
  log.printEnd( " OK" );

  OZ_GL_CHECK_ERROR();

  log.unindent();
  log.println( "}" );
}

}
}
