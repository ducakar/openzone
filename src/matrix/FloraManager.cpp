/*
 *  FloraManager.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "FloraManager.h"

#include "Synapse.h"
#include "Collider.h"

namespace oz
{

  FloraManager floraManager;

  inline void FloraManager::addTree( float x, float y )
  {
    Vec3 pos = Vec3( x, y, world.terra.height( x, y ) );

    const char *type;

    if( pos.z > 60.0f ) {
      return;
    }
    else if( pos.z > 40.0f ) {
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

    if( collider.testOSO( bounds ) ) {
      synapse.addObject( type, pos );
    }
  }

  void FloraManager::seed()
  {
    float area = 4.0f * World::DIM * World::DIM * DENSITY;

    number = static_cast<int>( area * DENSITY );
    growth = static_cast<int>( area * GROWTH );

    for( int i = 0; i < number; i++ ) {
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
