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
    float z = world.terra.height( x, y );

    const char *type;
    float bias;

    if( z > 20.0f ) {
      type = "Tree3";
      bias = 7.5f;
    }
    else if( z > 5.0f ) {
      type = "Tree2";
      bias = 3.5f;
    }
    else {
      return;
    }

    Object *tree = translator.createObject( type, Vec3( x, y, z + bias ) );

    if( collider.testOSO( *tree ) ) {
      synapse.put( tree );
    }
    else {
      delete tree;
    }
  }

  void FloraManager::seed()
  {
    float area = 4.0f * World::DIM * World::DIM * DENSITY;

    number = (int) ( area * DENSITY );
    growth = (int) ( area * GROWTH );

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
