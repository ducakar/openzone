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

  Synapse::Synapse() : isClient( false )
  {}

  void Synapse::clear()
  {
    world.clear();
    bsps.clear();
    structs.clear();
    objects.clear();
    particles.clear();
  }

}
