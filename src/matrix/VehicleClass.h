/*
 *  VehicleClass.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *
 *  $Id$
 */

#pragma once

#include "DynObjectClass.h"
#include "Vehicle.h"

namespace oz
{

  // TODO VehicleClass
  struct VehicleClass : DynObjectClass
  {
    static Class *init( Config *config );

    Object *create( const Vec3 &pos );
  };

}
