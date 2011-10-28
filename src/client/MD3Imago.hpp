/*
 *  MD3Imago.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/Imago.hpp"
#include "client/MD3.hpp"

namespace oz
{
namespace client
{

  class MD3Imago : public Imago
  {
    public:

      static const float TURN_SMOOTHING_COEF;

      MD3*           md3;
      float          h;
//       MD3::AnimState anim;

      static Pool<MD3Imago> pool;

    protected:

      MD3Imago()
      {
        flags |= Imago::MD2MODEL_BIT;
      }

      virtual ~MD3Imago();

    public:

      static Imago* create( const Object* obj );

//       void setAnim( Anim::Type anim );
      virtual void draw( const Imago* parent, int mask );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
