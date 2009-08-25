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

  Synapse::Synapse() : nObjects( 0 ), isSingle( true ), isServer( false ), isClient( false )
  {}

  void Synapse::commitPlus()
  {
    // put
    foreach( i, putObjects.iterator() ) {
      world.put( *i );
    }
    foreach( i, useActions.iterator() ) {
      i->target->onUse( i->user );
    }

    nObjects = world.objects.length();
  }

  void Synapse::commitMinus()
  {
    // cut
    foreach( i, cutStructs.iterator() ) {
      world.unposition( *i );
      world.cut( *i );
    }
    foreach( i, cutObjects.iterator() ) {
      world.unposition( *i );
      world.cut( *i );
    }
    foreach( i, cutParts.iterator() ) {
      world.unposition( *i );
      world.cut( *i );
    }

    // remove (cut & delete)
    foreach( i, removeStructs.iterator() ) {
      world.unposition( *i );
      world.cut( *i );
      delete *i;
    }
    foreach( i, removeObjects.iterator() ) {
      world.unposition( *i );
      world.cut( *i );
      delete *i;
    }
    foreach( i, removeParts.iterator() ) {
      world.unposition( *i );
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
    putObjects.clear();

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
