/*
 *  FloraManager.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/FloraManager.hpp"

#include "matrix/Collider.hpp"
#include "matrix/Synapse.hpp"

namespace oz
{

  // plants/m2
  const float FloraManager::DENSITY = 0.04f;
  // dim * SPACING
  const float FloraManager::SPACING = 8.0f;
  // plants/m2/s
  const float FloraManager::GROWTH =  0.0001f;

  FloraManager floraManager;

  inline void FloraManager::addTree( float x, float y )
  {
    Vec3 pos = Vec3( x, y, world.terra.height( x, y ) );

    const char* type;

    if( pos.z > 65.0f ) {
      return;
    }
    else if( pos.z > 45.0f ) {
      type = "Tree3";
      pos.z += 7.0f;
    }
    else if( pos.z > 20.0f ) {
      type = "Tree2";
      pos.z += 3.0f;
    }
    else if( pos.z > 2.0f ) {
      type = "Tree1";
      pos.z += 8.0f;
    }
    else {
      return;
    }

    AABB bounds = AABB( pos, translator.classes[type]->dim );
    bounds *= SPACING;

    if( !collider.overlapsOSO( bounds ) ) {
      synapse.addObject( type, pos );
    }
  }

  inline void FloraManager::addPlant( const char* type, float x, float y )
  {
    Vec3 pos = Vec3( x, y, world.terra.height( x, y ) );
    AABB bounds = AABB( pos, translator.classes[type]->dim );

    if( pos.z < 0.0f || 40.0f < pos.z ) {
      return;
    }

    if( !collider.overlapsOSO( bounds ) ) {
      synapse.addObject( type, pos );
    }
  }

  void FloraManager::seed()
  {
    float area = 4.0f * World::DIM * World::DIM * DENSITY;

    number = int( area * DENSITY );
    growth = int( area * GROWTH );

    for( int i = 0; i < number; ++i ) {
      float x = Math::frand() * 2.0f * World::DIM - World::DIM;
      float y = Math::frand() * 2.0f * World::DIM - World::DIM;

      addTree( x, y );
    }
  }

  void FloraManager::clear()
  {
    plants.clear();
  }

  void FloraManager::update()
  {}

}
