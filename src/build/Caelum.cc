/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file build/Caelum.cc
 */

#include "stable.hh"

#include "build/Caelum.hh"

#include "client/Caelum.hh"
#include "client/OpenGL.hh"

#include "build/Mesh.hh"
#include "build/Context.hh"

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
    while( Math::isnan( length ) || length < 2500.0f || length > 10000.0f );
  }

  BufferStream os;

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
  File( destPath ).write( &os );
  log.printEnd( " OK" );

  OZ_GL_CHECK_ERROR();

  log.unindent();
  log.println( "}" );
}

}
}
