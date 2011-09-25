/*
 *  MD2WeaponModel.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/MD2Model.hpp"

namespace oz
{
namespace client
{

  class MD2WeaponModel : public MD2Model
  {
    public:

      static Pool<MD2WeaponModel> pool;

      static Model* create( const Object* obj );

      virtual void draw( const Model* parent );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
