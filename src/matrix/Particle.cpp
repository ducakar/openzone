/*
 *  Particle.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Particle.h"

namespace oz
{

  const float Particle::MAX_ROTVELOCITY = 1000.0f;

  // serialize whole object
  void Particle::serialize( char* ) const
  {}

      // serialize only information necessary for network synchronizing
  void Particle::updateSerialize( char* ) const
  {}

      // update object with recieved synchronization information from network
  void Particle::updateDeserialize( const char* )
  {}

}
