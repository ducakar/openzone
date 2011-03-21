/*
 *  MD2Model.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/Model.hpp"
#include "client/MD2.hpp"

namespace oz
{
namespace client
{

  class MD2Model : public Model
  {
    public:

      static const float TURN_SMOOTHING_COEF;

      MD2*           md2;
      float          h;
      MD2::AnimState anim;

      static Pool<MD2Model> pool;

    protected:

      explicit MD2Model()
      {
        flags |= Model::MD2MODEL_BIT;
      }

      virtual ~MD2Model();

    public:

      static Model* create( const Object* obj );

      void setAnim( Anim::Type anim );
      virtual void draw( const Model* parent );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
