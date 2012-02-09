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
 * @file build/Caelum.cc
 */

#include "stable.hh"

#include "build/Caelum.hh"

#include "client/Caelum.hh"

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

  File destFile( String::str( "caelum/%s.ozcCaelum", name ) );

  BufferStream os;

  for( int i = 0; i < client::Caelum::MAX_STARS; ++i ) {
    Vec3 p;
    float length;

    do {
      p = 100.0f * Vec3( Math::centralRand(), Math::centralRand(), Math::centralRand() );
      length = p.sqL();
    }
    while( Math::isnan( length ) || length < 2500.0f || length > 10000.0f );

    Vec3 z = ~p;
    Vec3 x = ~Vec3( z.z, 0.0f, -z.x );
    Vec3 y = z ^ x;

    Mat44 transf = Mat44( x, y, z, -p );

    Point3 corners[4] = {
      transf * Point3( -STAR_DIM, 0.0f, 0.0f ),
      transf * Point3( 0.0f, -STAR_DIM, 0.0f ),
      transf * Point3( +STAR_DIM, 0.0f, 0.0f ),
      transf * Point3( 0.0f, +STAR_DIM, 0.0f )
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

  uint texId = context.loadRawTexture( "caelum/sun.png", false );
  context.writeTexture( texId, &os );
  glDeleteTextures( 1, &texId );

  texId = context.loadRawTexture( "caelum/moon.png", false );
  context.writeTexture( texId, &os );
  glDeleteTextures( 1, &texId );

  log.print( "Dumping into '%s' ...", destFile.path().cstr() );

  if( !destFile.write( &os ) ) {
    throw Exception( "Failed to write '%s'", destFile.path().cstr() );
  }

  log.printEnd( " OK" );

  OZ_GL_CHECK_ERROR();

  log.unindent();
  log.println( "}" );
}

}
}
