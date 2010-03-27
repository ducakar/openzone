/*
 *  OBJVehicleModel.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "client/Model.h"

namespace oz
{
namespace client
{

  class OBJVehicleModel : public Model
  {
    protected:

      uint list;

    public:

      static Pool<OBJVehicleModel, 0, 256> pool;

      static Model* create( const Object* obj );

      virtual ~OBJVehicleModel();

      virtual void draw( const Model* parent );

    OZ_STATIC_POOL_ALLOC( pool );

  };

}
}
