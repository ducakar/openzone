/*
 *  Synapse.cc
 *
 *  World manipulation interface.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hh"

#include "matrix/Synapse.hh"

namespace oz
{

  Synapse synapse;

  Synapse::Synapse() : isSingle( true ), isServer( false ), isClient( false )
  {}

  void Synapse::genParts( int number, const Vec3& p,
                          const Vec3& velocity, float velocitySpread,
                          const Vec3& colour, float colorSpread,
                          float restitution, float mass, float lifeTime )
  {
    float velocitySpread2 = velocitySpread / 2.0f;
    float colourSpread2 = colorSpread / 2.0f;

    for( int i = 0; i < number; ++i ) {
      Vec3 velDisturb = Vec3( velocitySpread * Math::frand() - velocitySpread2,
                              velocitySpread * Math::frand() - velocitySpread2,
                              velocitySpread * Math::frand() - velocitySpread2 );
      Vec3 colourDisturb = Vec3( colorSpread * Math::frand() - colourSpread2,
                                 colorSpread * Math::frand() - colourSpread2,
                                 colorSpread * Math::frand() - colourSpread2 );
      float timeDisturb = lifeTime * Math::frand();

      addPart( p, velocity + velDisturb, colour + colourDisturb,
               restitution, mass, 0.5f * lifeTime + timeDisturb );
    }
  }

  void Synapse::update()
  {
    deleteObjects.free();

    actions.clear();

    addedStructs.clear();
    addedObjects.clear();
    addedParts.clear();

    removedStructs.clear();
    removedObjects.clear();
    removedParts.clear();
  }

  void Synapse::clearTickets()
  {
    putStructsIndices.clear();
    putObjectsIndices.clear();
    putPartsIndices.clear();
  }

}
