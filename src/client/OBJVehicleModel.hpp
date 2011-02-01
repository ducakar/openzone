/*
 *  OBJVehicleModel.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/Model.hpp"
#include "client/OBJ.hpp"

namespace oz
{
namespace client
{

  class OBJVehicleModel : public Model
  {
    protected:

      OBJ* objModel;

    public:

      static Pool<OBJVehicleModel, 256> pool;

      static Model* create( const Object* obj );

      virtual ~OBJVehicleModel();

      virtual void draw( const Model* parent );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
