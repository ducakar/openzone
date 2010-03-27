/*
 *  MD2Model.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "client/Model.h"
#include "client/MD2.h"

namespace oz
{
namespace client
{

  class MD2Model : public Model
  {
    public:

      MD2*           md2;
      MD2::AnimState anim;

      static Pool<MD2Model, 0, 256> pool;

    protected:

      MD2Model()
      {
        flags |= Model::MD2MODEL_BIT;
      }

      virtual ~MD2Model();

    public:

      static Model* create( const Object* obj );

      void setAnim( int type );
      virtual void draw( const Model* parent );

    OZ_STATIC_POOL_ALLOC( pool );

  };

}
}
