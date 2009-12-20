/*
 *  VehicleClass.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "DynamicClass.h"
#include "Vehicle.h"

namespace oz
{

  struct VehicleClass : DynamicClass
  {
    static const int BASE_FLAGS = Object::DYNAMIC_BIT | Object::HIT_FUNC_BIT | Object::USE_FUNC_BIT |
        Object::UPDATE_FUNC_BIT | Object::VEHICLE_BIT;
    static const int DEFAULT_FLAGS = Object::CLIP_BIT;

    int   state;

    Vec3  crewPos[Vehicle::CREW_MAX];
    Quat  crewRot[Vehicle::CREW_MAX];

    float moveMomentum;

    static ObjectClass *init( const String &name, const Config *config );

    virtual Object *create( int index, const Vec3 &pos );
    virtual Object *create( int index, InputStream *istream );
  };

}
