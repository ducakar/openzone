/*
 *  MD2WeaponModel.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "MD2Model.h"

namespace oz
{
namespace client
{

  class MD2WeaponModel : public MD2Model
  {
    public:

      static Pool<MD2WeaponModel, 0, 256> pool;

      static Model* create( const Object* obj );

      virtual void draw( const Model* parent );

    OZ_STATIC_POOL_ALLOC( pool );

  };

}
}
