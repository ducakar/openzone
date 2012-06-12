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
  Log::println( "Prebuilding Caelum '%s' {", name );
  Log::indent();

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

    Point corners[4] = {
      transf * Point( -STAR_DIM, 0.0f, 0.0f ),
      transf * Point( 0.0f, -STAR_DIM, 0.0f ),
      transf * Point( +STAR_DIM, 0.0f, 0.0f ),
      transf * Point( 0.0f, +STAR_DIM, 0.0f )
    };

    os.writeFloat( corners[0].x );
    os.writeFloat( corners[0].y );
    os.writeFloat( corners[0].z );

    os.writeFloat( corners[1].x );
    os.writeFloat( corners[1].y );
    os.writeFloat( corners[1].z );

    os.writeFloat( corners[2].x );
    os.writeFloat( corners[2].y );
    os.writeFloat( corners[2].z );

    os.writeFloat( corners[3].x );
    os.writeFloat( corners[3].y );
    os.writeFloat( corners[3].z );
  }

  for( int i = 0; i < client::Caelum::MAX_STARS; ++i ) {
    os.writeUShort( ushort( i * 4 + 0 ) );
    os.writeUShort( ushort( i * 4 + 0 ) );
    os.writeUShort( ushort( i * 4 + 1 ) );
    os.writeUShort( ushort( i * 4 + 2 ) );
    os.writeUShort( ushort( i * 4 + 3 ) );
    os.writeUShort( ushort( i * 4 + 3 ) );
  }

  uint texId = context.loadLayer( "caelum/sun.png", false );
  context.writeLayer( texId, &os );
  glDeleteTextures( 1, &texId );

  texId = context.loadLayer( "caelum/moon.png", false );
  context.writeLayer( texId, &os );
  glDeleteTextures( 1, &texId );

  Log::print( "Dumping into '%s' ...", destFile.path().cstr() );

  if( !destFile.write( os.begin(), os.length() ) ) {
    throw Exception( "Failed to write '%s'", destFile.path().cstr() );
  }

  Log::printEnd( " OK" );

  OZ_GL_CHECK_ERROR();

  Log::unindent();
  Log::println( "}" );
}

}
}
