/*
 *  Synapse.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Synapse.h"

namespace oz
{

  Synapse synapse;

  Synapse::Synapse() : isSingle( true ), isServer( false ), isClient( false )
  {}

  void Synapse::genParts( int number, const Vec3 &p,
                          const Vec3 &velocity, float velocitySpread,
                          float rejection, float mass, float lifeTime,
                          float size, const Vec3 &color, float colorSpread )
  {
    float velocitySpread2 = velocitySpread / 2.0f;
    float colorSpread2 = colorSpread / 2.0f;

    for( int i = 0; i < number; i++ ) {
      Vec3 velDisturb = Vec3( velocitySpread * Math::frand() - velocitySpread2,
                              velocitySpread * Math::frand() - velocitySpread2,
                              velocitySpread * Math::frand() - velocitySpread2 );
      Vec3 colorDisturb = Vec3( colorSpread * Math::frand() - colorSpread2,
                                colorSpread * Math::frand() - colorSpread2,
                                colorSpread * Math::frand() - colorSpread2 );
      float timeDisturb = lifeTime * Math::frand();

      addPart( p, velocity + velDisturb, rejection, mass, 0.5f * lifeTime + timeDisturb, size,
               color + colorDisturb );
    }
  }

  void Synapse::commit()
  {
    foreach( i, actions.iterator() ) {
      i->target->use( i->user );
    }
    actions.clear();

    foreach( i, cutStructs.iterator() ) {
      world.unposition( *i );
      world.cut( *i );
    }
    foreach( i, cutObjects.iterator() ) {
      world.unposition( *i );
      world.cut( *i );
    }

    cutStructs.clear();
    cutObjects.clear();
  }

  void Synapse::doDeletes()
  {
    foreach( i, deleteStructs.iterator() ) {
      delete *i;
    }
    foreach( i, deleteObjects.iterator() ) {
      delete *i;
    }

    deleteStructs.clear();
    deleteObjects.clear();
  }

  void Synapse::clear()
  {
    putStructs.clear();
    putObjects.clear();
  }

  void Synapse::clearTickets()
  {
    putStructsIndices.clear();
    putObjectsIndices.clear();
    putPartsIndices.clear();
  }

}
