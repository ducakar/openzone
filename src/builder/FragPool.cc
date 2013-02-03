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
 * @file builder/FragPool.cc
 */

#include <stable.hh>
#include <builder/FragPool.hh>

#include <builder/Context.hh>

#define SET_FLAG( flagBit, varName, defValue ) \
  if( config[varName].get( defValue ) ) { \
    flags |= flagBit; \
  }

namespace oz
{
namespace builder
{

void FragPool::build( OutputStream* os, const char* className )
{
  File configFile( File::VIRTUAL, String::str( "frag/%s.json", className ) );

  JSON config;
  if( !config.load( configFile ) ) {
    OZ_ERROR( "Failed to load '%s'", configFile.path().cstr() );
  }

  int flags = 0;

  SET_FLAG( matrix::FragPool::FADEOUT_BIT, "flag.fadeout", true );

  float velocitySpread = config["velocitySpread"].get( 4.0f );

  if( velocitySpread < 0.0f ) {
    OZ_ERROR( "%s: Frag velocitySpread must be >= 0.0", className );
  }

  float life       = config["life"].get( 4.0f );
  float lifeSpread = config["lifeSpread"].get( 1.0f );
  float mass       = config["mass"].get( 0.0f );
  float elasticity = config["elasticity"].get( 0.5f );

  if( life <= 0.0f ) {
    OZ_ERROR( "%s: Frag life must be > 0.0", className );
  }
  if( lifeSpread < 0.0f ) {
    OZ_ERROR( "%s: Frag lifeSpread must be >= 0.0", className );
  }
  if( mass < 0.0f ) {
    OZ_ERROR( "%s: Frag mass must be >= 0.0", className );
  }
  if( elasticity < 0.0f || 1.0f < elasticity ) {
    OZ_ERROR( "%s: Frag elasticity must lie on interval [0, 1]", className );
  }

  const JSON& modelsConfig = config["models"];
  int nModels = modelsConfig.length();

  List<String> models;

  for( int i = 0; i < nModels; ++i ) {
    const char* modelName = modelsConfig[i].get( "" );

    if( String::isEmpty( modelName ) ) {
      OZ_ERROR( "%s: Empty name for model #%d", className, i );
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
  os->writeFloat( elasticity );

  os->writeInt( models.length() );
  foreach( i, models.citer() ) {
    os->writeString( *i );
  }

  config.clear( true );
}

FragPool fragPool;

}
}
