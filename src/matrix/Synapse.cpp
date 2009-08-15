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

  void Synapse::clear()
  {
    clearPending();
    clearTickets();
  }

}
