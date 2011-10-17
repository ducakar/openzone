/*
 *  MD3Model.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/Model.hpp"
#include "client/MD3.hpp"

namespace oz
{
namespace client
{

  class MD3Model : public Model
  {
    public:

      static const float TURN_SMOOTHING_COEF;

      MD3*           md3;
      float          h;
//       MD3::AnimState anim;

      static Pool<MD3Model> pool;

    protected:

      MD3Model()
      {
        flags |= Model::MD2MODEL_BIT;
      }

      virtual ~MD3Model();

    public:

      static Model* create( const Object* obj );

//       void setAnim( Anim::Type anim );
      virtual void draw( const Model* parent );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
