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
 * @file build/FragPool.cc
 */

#include "build/FragPool.hh"

#include "build/Context.hh"

#define SET_FLAG( flagBit, varName, defValue ) \
  if( config[varName].get( defValue ) ) { \
    flags |= flagBit; \
  }

namespace oz
{
namespace build
{

FragPool fragPool;

void FragPool::build( BufferStream* os, const char* className )
{
  PFile configFile( String::str( "frag/%s.json", className ) );

  JSON config;
  if( !config.load( configFile ) ) {
    throw Exception( "Failed to load '%s'", configFile.path().cstr() );
  }

  int flags = 0;

  SET_FLAG( matrix::FragPool::FADEOUT_BIT, "flag.fadeout", true );

  float velocitySpread = config["velocitySpread"].get( 4.0f );

  if( velocitySpread < 0.0f ) {
    throw Exception( "%s: Frag velocitySpread must be >= 0.0", className );
  }

  float life        = config["life"].get( 4.0f );
  float lifeSpread  = config["lifeSpread"].get( 1.0f );
  float mass        = config["mass"].get( 0.0f );
  float restitution = config["restitution"].get( 0.5f );

  if( life <= 0.0f ) {
    throw Exception( "%s: Frag life must be > 0.0", className );
  }
  if( lifeSpread < 0.0f ) {
    throw Exception( "%s: Frag lifeSpread must be >= 0.0", className );
  }
  if( mass < 0.0f ) {
    throw Exception( "%s: Frag mass must be >= 0.0", className );
  }
  if( restitution < 0.0f || 1.0f < restitution ) {
    throw Exception( "%s: Frag restitution must lie on interval [0, 1]", className );
  }

  const JSON& modelsConfig = config["models"];
  int nModels = modelsConfig.length();

  Vector<String> models;

  for( int i = 0; i < nModels; ++i ) {
    const char* modelName = modelsConfig[i].get( "" );

    if( String::isEmpty( modelName ) ) {
      throw Exception( "%s: Empty name for model #%d", className, i );
    }

    models.add( modelName );

    context.usedModels.include( modelName );
  }

  os->writeString( className );

  os->writeInt( flags );

  os->writeFloat( velocitySpread );

  os->writeFloat( life );
  os->writeFloat( lifeSpread );

  os->writeFloat( mass );
  os->writeFloat( restitution );

  os->writeInt( models.length() );
  foreach( i, models.citer() ) {
    os->writeString( *i );
  }

  config.clear( true );
}

}
}
