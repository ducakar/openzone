/*
 *  FloraModule.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/modules/FloraModule.hpp"

#include "matrix/Translator.hpp"
#include "matrix/Collider.hpp"
#include "matrix/Synapse.hpp"

#include <lua.hpp>

namespace oz
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
      type = "smreka";
      pos.z += 7.0f;
    }
    else if( pos.z > 30.0f ) {
      type = "drevo";
      pos.z += 3.0f;
    }
    else if( pos.z > 2.0f ) {
      type = "palma";
      pos.z += 8.0f;
    }
    else {
      return;
    }

    AABB bounds = AABB( pos, translator.classes.get( type )->dim );
    bounds *= SPACING;

    if( !collider.overlapsOSO( bounds ) ) {
      synapse.addObject( type, pos );
    }
  }

  void FloraModule::addPlant( const char* type, float x, float y )
  {
    Point3 pos    = Point3( x, y, orbis.terra.height( x, y ) );
    AABB   bounds = AABB( pos, translator.classes.get( type )->dim );

    if( pos.z < 0.0f || 40.0f < pos.z ) {
      return;
    }

    if( !collider.overlapsOSO( bounds ) ) {
      synapse.addObject( type, pos );
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
  }

  void FloraModule::onRegister()
  {
    OZ_LUA_REGISTER_FUNC( ozFloraGetDensity );
    OZ_LUA_REGISTER_FUNC( ozFloraSetDensity );
    OZ_LUA_REGISTER_FUNC( ozFloraGetSpacing );
    OZ_LUA_REGISTER_FUNC( ozFloraSetSpacing );
    OZ_LUA_REGISTER_FUNC( ozFloraGetNumber );
    OZ_LUA_REGISTER_FUNC( ozFloraSeed );
  }

  int FloraModule::ozFloraGetDensity( lua_State* l )
  {
    lua_pushnumber( l, floraModule.density );
    return 1;
  }

  int FloraModule::ozFloraSetDensity( lua_State* l )
  {
    floraModule.density = float( lua_tonumber( l, 1 ) );
    return 0;
  }

  int FloraModule::ozFloraGetSpacing( lua_State* l )
  {
    lua_pushnumber( l, floraModule.spacing );
    return 1;
  }

  int FloraModule::ozFloraSetSpacing( lua_State* l )
  {
    float spacing = float( lua_tonumber( l, 1 ) );
    if( spacing < 0 ) {
      throw Exception( "Lua::ozFloraGetSpacing: spacing must be >= 0.0" );
    }

    floraModule.spacing = spacing;
    return 0;
  }

  int FloraModule::ozFloraGetNumber( lua_State* l )
  {
    lua_pushnumber( l, floraModule.number );
    return 1;
  }

  int FloraModule::ozFloraSeed( lua_State* )
  {
    floraModule.seed();
    return 0;
  }

}
