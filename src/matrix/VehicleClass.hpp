/*
 *  VehicleClass.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/DynamicClass.hpp"
#include "matrix/Vehicle.hpp"

namespace oz
{

  class VehicleClass : public DynamicClass
  {
    private:

      static const int BASE_FLAGS = Object::DYNAMIC_BIT | Object::HIT_FUNC_BIT |
          Object::USE_FUNC_BIT | Object::UPDATE_FUNC_BIT | Object::VEHICLE_BIT;
      static const int DEFAULT_FLAGS = Object::SOLID_BIT;

    public:

      int    type;
      int    state;

      String onShot[Vehicle::WEAPONS_MAX];
      int    nShots[Vehicle::WEAPONS_MAX];
      float  shotInterval[Vehicle::WEAPONS_MAX];

      Vec3   crewPos[Vehicle::CREW_MAX];
      Quat   crewRot[Vehicle::CREW_MAX];

      float  moveMomentum;

      float  hoverHeight;
      float  hoverHeightStiffness;
      float  hoverMomentumStiffness;

      static ObjectClass* init( const String& name, const Config* config );

      virtual Object* create( int index, const Point3& pos ) const;
      virtual Object* create( int index, InputStream* istream ) const;

  };

}
