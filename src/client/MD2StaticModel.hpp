/*
 *  MD2StaticModel.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/Model.hpp"

namespace oz
{
namespace client
{

  class MD2;

  class MD2StaticModel : public Model
  {
    protected:

      MD2* md2;

      virtual ~MD2StaticModel();

    public:

      static Pool<MD2StaticModel, 256> pool;

      static Model* create( const Object* obj );

      virtual void draw( const Model* parent );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
