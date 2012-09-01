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
 * @file modules/flora/FloraModule.cc
 */

#include <stable.hh>
#include <modules/flora/FloraModule.hh>

#include <matrix/Library.hh>
#include <matrix/Synapse.hh>

#include <client/Lua.hh>

#include <common/luabase.hh>

using namespace oz::client;

namespace oz
{
namespace modules
{

// plants/m2
const float FloraModule::DENSITY    = 0.04f;
// dim * SPACING
const float FloraModule::SPACING    = 16.0f;
const float FloraModule::TREE_DEPTH = 0.75f;

FloraModule floraModule;

void FloraModule::addTree( float x, float y )
{
  Point pos = Point( x, y, orbis.terra.height( x, y ) );

  const char* type;

  if( pos.z > 110.0f ) {
    return;
  }
  else if( pos.z > 70.0f ) {
    type = "pine";
  }
  else if( pos.z > 30.0f ) {
    type = "tree";
  }
  else if( pos.z > 2.0f ) {
    type = "palm";
  }
  else {
    return;
  }

  const ObjectClass* clazz = library.objClass( type );

  pos.z += clazz->dim.z + 2.0f * EPSILON;
  AABB bounds = AABB( pos, Vec3( clazz->dim.x * SPACING, clazz->dim.y * SPACING, clazz->dim.z ) );

  if( !client::collider.overlaps( bounds ) ) {
    pos.z -= TREE_DEPTH;
    synapse.addObject( type, pos, Heading( Math::rand( 4 ) ), true );
    ++number;
  }
}

void FloraModule::addPlant( const char* type, float x, float y )
{
  const ObjectClass* clazz = library.objClass( type );

  Point pos    = Point( x, y, orbis.terra.height( x, y ) + clazz->dim.z + 2.0f * EPSILON );
  AABB  bounds = AABB( pos, clazz->dim );

  if( pos.z < 0.0f || 40.0f < pos.z ) {
    return;
  }

  if( !client::collider.overlaps( bounds ) ) {
    pos.z -= TREE_DEPTH;
    synapse.addObject( type, pos, Heading( Math::rand( 4 ) ), true );
    ++number;
  }
}

void FloraModule::seed()
{
  float area = 4.0f * Orbis::DIM * Orbis::DIM * DENSITY;

  int n = int( area * DENSITY );

  for( int i = 0; i < n; ++i ) {
    float x = Math::rand() * 2.0f * Orbis::DIM - Orbis::DIM;
    float y = Math::rand() * 2.0f * Orbis::DIM - Orbis::DIM;

    addTree( x, y );
  }

  Log::println( "Flora seeded %d trees", number );
}

void FloraModule::load()
{
  number = 0;
}

void FloraModule::registerLua() const
{
  IMPORT_FUNC( ozFloraGetDensity );
  IMPORT_FUNC( ozFloraSetDensity );
  IMPORT_FUNC( ozFloraGetSpacing );
  IMPORT_FUNC( ozFloraSetSpacing );
  IMPORT_FUNC( ozFloraGetNumber );
  IMPORT_FUNC( ozFloraSeed );
}

int FloraModule::ozFloraGetDensity( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( floraModule.density );
  return 1;
}

int FloraModule::ozFloraSetDensity( lua_State* l )
{
  ARG( 1 );

  floraModule.density = l_tofloat( 1 );
  return 0;
}

int FloraModule::ozFloraGetSpacing( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( floraModule.spacing );
  return 1;
}

int FloraModule::ozFloraSetSpacing( lua_State* l )
{
  ARG( 1 );

  float spacing = l_tofloat( 1 );
  if( spacing < 0 ) {
    OZ_ERROR( "Lua::ozFloraGetSpacing: spacing must be >= 0.0" );
  }

  floraModule.spacing = spacing;
  return 0;
}

int FloraModule::ozFloraGetNumber( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( floraModule.number );
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
