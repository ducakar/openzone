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

  String destPath = String::str( "caelum/%s.ozcCaelum", name );

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

    Mat44 transf = Mat44( x, y, z, positions[i] - Point3::ORIGIN );

    Vec3 corners[4] = {
      transf * Vec3( -STAR_DIM, 0.0f, 0.0f ),
      transf * Vec3( 0.0f, -STAR_DIM, 0.0f ),
      transf * Vec3( +STAR_DIM, 0.0f, 0.0f ),
      transf * Vec3( 0.0f, +STAR_DIM, 0.0f )
    };

    client::Vertex vertex[4] = {
      { { corners[0].x, corners[0].y, corners[0].z }, {}, {}, {}, {}, {} },
      { { corners[1].x, corners[1].y, corners[1].z }, {}, {}, {}, {}, {} },
      { { corners[2].x, corners[2].y, corners[2].z }, {}, {}, {}, {}, {} },
      { { corners[3].x, corners[3].y, corners[3].z }, {}, {}, {}, {}, {} }
    };

    vertex[0].write( &os );
    vertex[1].write( &os );
    vertex[2].write( &os );
    vertex[3].write( &os );
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
