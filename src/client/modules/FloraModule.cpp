/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
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
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file client/modules/FloraModule.cpp
 */

#include "stable.hpp"

#include "client/modules/FloraModule.hpp"

#include "matrix/Library.hpp"
#include "matrix/Collider.hpp"
#include "matrix/Synapse.hpp"

#include "client/Lua.hpp"

#include "luamacros.hpp"

namespace oz
{
namespace client
{

// plants/m2
const float FloraModule::DENSITY = 0.04f;
// dim * SPACING
const float FloraModule::SPACING = 12.0f;

FloraModule floraModule;

void FloraModule::addTree( float x, float y )
{
  Point3 pos = Point3( x, y, orbis.terra.height( x, y ) );

  const char* type;

  if( pos.z > 110.0f ) {
    return;
  }
  else if( pos.z > 70.0f ) {
    type = "pine";
    pos.z += 7.0f;
  }
  else if( pos.z > 30.0f ) {
    type = "tree";
    pos.z += 3.0f;
  }
  else if( pos.z > 2.0f ) {
    type = "palm";
    pos.z += 8.0f;
  }
  else {
    return;
  }

  const ObjectClass* const* clazz = library.classes.find( type );
  if( clazz == null ) {
    throw Exception( "Object class '" + String( type ) + "' missing" );
  }

  AABB bounds = AABB( pos, ( *clazz )->dim );
  bounds *= SPACING;

  if( !collider.overlapsOSO( bounds ) ) {
    synapse.addObject( type, pos, Heading( Math::rand( 4 ) ) );
    ++number;
  }
}

void FloraModule::addPlant( const char* type, float x, float y )
{
  const ObjectClass* const* clazz = library.classes.find( type );
  if( clazz == null ) {
    throw Exception( "Object class '" + String( type ) + "' missing" );
  }

  Point3 pos    = Point3( x, y, orbis.terra.height( x, y ) );
  AABB   bounds = AABB( pos, ( *clazz )->dim );

  if( pos.z < 0.0f || 40.0f < pos.z ) {
    return;
  }

  if( !collider.overlapsOSO( bounds ) ) {
    synapse.addObject( type, pos, Heading( Math::rand( 4 ) ) );
    ++number;
  }
}

void FloraModule::seed()
{
  float area = 4.0f * Orbis::DIM * Orbis::DIM * DENSITY;

  number = int( area * DENSITY );

  for( int i = 0; i < number; ++i ) {
    float x = Math::rand() * 2.0f * Orbis::DIM - Orbis::DIM;
    float y = Math::rand() * 2.0f * Orbis::DIM - Orbis::DIM;

    addTree( x, y );
  }

  log.println( "Flora seeded %d trees", number );
}

void FloraModule::load()
{
  number = 0;
}

void FloraModule::registerLua() const
{
  OZ_LUA_FUNC( ozFloraGetDensity );
  OZ_LUA_FUNC( ozFloraSetDensity );
  OZ_LUA_FUNC( ozFloraGetSpacing );
  OZ_LUA_FUNC( ozFloraSetSpacing );
  OZ_LUA_FUNC( ozFloraGetNumber );
  OZ_LUA_FUNC( ozFloraSeed );
}

int FloraModule::ozFloraGetDensity( lua_State* l )
{
  ARG( 0 );

  pushfloat( floraModule.density );
  return 1;
}

int FloraModule::ozFloraSetDensity( lua_State* l )
{
  ARG( 1 );

  floraModule.density = tofloat( 1 );
  return 0;
}

int FloraModule::ozFloraGetSpacing( lua_State* l )
{
  ARG( 0 );

  pushfloat( floraModule.spacing );
  return 1;
}

int FloraModule::ozFloraSetSpacing( lua_State* l )
{
  ARG( 1 );

  float spacing = tofloat( 1 );
  if( spacing < 0 ) {
    throw Exception( "Lua::ozFloraGetSpacing: spacing must be >= 0.0" );
  }

  floraModule.spacing = spacing;
  return 0;
}

int FloraModule::ozFloraGetNumber( lua_State* l )
{
  ARG( 0 );

  pushfloat( floraModule.number );
  return 1;
}

int FloraModule::ozFloraSeed( lua_State* l )
{
  ARG( 0 );

  floraModule.seed();
  return 0;
}

}
}
