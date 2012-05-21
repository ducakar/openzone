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
 * @file matrix/FragPool.cc
 */

#include "stable.hh"

#include "matrix/FragPool.hh"

#include "matrix/Frag.hh"
#include "matrix/Library.hh"

#define OZ_FRAG_SET_FLAG( flagBit, varName, defValue ) \
  if( fragConfig.get( varName, defValue ) ) { \
    flags |= flagBit; \
  }

namespace oz
{
namespace matrix
{

FragPool::FragPool( const char* name_, int id_ ) :
  name( name_ ), id( id_ )
{
  PFile file( "frag/" + name + ".rc" );

  Config fragConfig;
  if( !fragConfig.load( file ) ) {
    throw Exception( "Failed to read config from '%s'", file.path().cstr() );
  }

  flags = 0;

  OZ_FRAG_SET_FLAG( FADEOUT_BIT, "flag.fadeout", true );

  velocitySpread = fragConfig.get( "velocitySpread", 4.0f );

  if( velocitySpread < 0.0f ) {
    throw Exception( "%s: Frag velocitySpread must be >= 0.0", name.cstr() );
  }

  life        = fragConfig.get( "life", 4.0f );
  lifeSpread  = fragConfig.get( "lifeSpread", 1.0f );
  mass        = fragConfig.get( "mass", 0.0f );
  restitution = fragConfig.get( "restitution", 1.5f );

  if( life <= 0.0f ) {
    throw Exception( "%s: Frag life must be > 0.0", name.cstr() );
  }
  if( lifeSpread < 0.0f ) {
    throw Exception( "%s: Frag lifeSpread must be >= 0.0", name.cstr() );
  }
  if( mass < 0.0f ) {
    throw Exception( "%s: Frag mass must be >= 0.0", name.cstr() );
  }
  if( restitution < 0.0f || 1.0f < restitution ) {
    throw Exception( "%s: Frag restitution must lie on interval [0, 1]", name.cstr() );
  }

  char buffer[] = "model  ";
  for( int i = 0; i < MAX_MODELS; ++i ) {
    hard_assert( i < 100 );

    buffer[ sizeof( buffer ) - 3 ] = char( '0' + ( i / 10 ) );
    buffer[ sizeof( buffer ) - 2 ] = char( '0' + ( i % 10 ) );

    const char* modelName = fragConfig.get( buffer, "" );
    if( !String::isEmpty( modelName ) ) {
      models.add( library.modelIndex( modelName ) );
    }
  }

  fragConfig.clear( true );
}

Frag* FragPool::create( int index, const Point& pos, const Vec3& velocity ) const
{
  return new Frag( this, index, pos, velocity );
}

Frag* FragPool::create( InputStream* istream ) const
{
  return new Frag( this, istream );
}

}
}
