/*
 *  LadderClass.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *
 *  $Id$
 */

#pragma once

#include "ObjectClass.h"
#include "Ladder.h"

namespace oz
{

  // TODO LadderClass
  struct LadderClass : ObjectClass
  {
    static ObjectClass *init( Config *config );
    virtual Object *create( const Vec3 &pos );
  };

}
