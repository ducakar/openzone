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

  Synapse::Synapse() : nStructs( 0 ), nObjects( 0 ), nParts( 0 ),
      isSingle( true ), isServer( false ), isClient( false )
  {}

  void Synapse::commitPlus()
  {
    // put
    foreach( i, putStructs.iterator() ) {
      world.put( *i );
    }
    foreach( i, putObjects.iterator() ) {
      world.put( *i );
    }
    foreach( i, putParts.iterator() ) {
      world.put( *i );
    }
    foreach( i, useActions.iterator() ) {
      i->target->onUse( i->user );
    }

    nStructs = world.structures.length();
    nObjects = world.objects.length();
    nParts   = world.particles.length();
  }

  void Synapse::commitMinus()
  {
    // cut
    foreach( i, cutStructs.iterator() ) {
      world.cut( *i );
    }
    foreach( i, cutObjects.iterator() ) {
      world.cut( *i );
    }
    foreach( i, cutParts.iterator() ) {
      world.cut( *i );
    }

    // remove (cut & delete)
    foreach( i, removeStructs.iterator() ) {
      world.cut( *i );
      delete *i;
    }
    foreach( i, removeObjects.iterator() ) {
      world.cut( *i );
      delete *i;
    }
    foreach( i, removeParts.iterator() ) {
      world.cut( *i );
      delete *i;
    }
  }

  void Synapse::commitAll()
  {
    commitPlus();
    commitMinus();
  }

  void Synapse::clearPending()
  {
    putStructs.clear();
    putObjects.clear();
    putParts.clear();

    cutStructs.clear();
    cutObjects.clear();
    cutParts.clear();

    removeStructs.clear();
    removeObjects.clear();
    removeParts.clear();

    useActions.clear();
  }

  void Synapse::clearTickets()
  {
    putStructsIndices.clear();
    putObjectsIndices.clear();
    putPartsIndices.clear();
  }

  void Synapse::clearAll()
  {
    clearPending();
    clearTickets();
  }

}
