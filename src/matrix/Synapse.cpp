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

  void Synapse::commit()
  {
    foreach( i, actions.iterator() ) {
      i->target->activate( i->action );
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
    foreach( i, cutParts.iterator() ) {
      world.unposition( *i );
      world.cut( *i );
    }

    cutStructs.clear();
    cutObjects.clear();
    cutParts.clear();
  }

  void Synapse::doDeletes()
  {
    foreach( i, deleteStructs.iterator() ) {
      delete *i;
    }
    foreach( i, deleteObjects.iterator() ) {
      delete *i;
    }
    foreach( i, deleteParts.iterator() ) {
      delete *i;
    }

    deleteStructs.clear();
    deleteObjects.clear();
    deleteParts.clear();
  }

  void Synapse::clear()
  {
    putStructs.clear();
    putObjects.clear();
    putParts.clear();
  }

  void Synapse::clearTickets()
  {
    putStructsIndices.clear();
    putObjectsIndices.clear();
    putPartsIndices.clear();
  }

}
