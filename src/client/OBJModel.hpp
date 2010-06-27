/*
 *  OBJModel.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

#include "client/Model.hpp"

namespace oz
{
namespace client
{

  class OBJModel : public Model
  {
    protected:

      uint list;

      virtual ~OBJModel();

    public:

      static Pool<OBJModel, 0, 256> pool;

      static Model* create( const Object* obj );

      virtual void draw( const Model* parent );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
