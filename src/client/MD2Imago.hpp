/*
 *  MD2Imago.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "client/Imago.hpp"
#include "client/MD2.hpp"

namespace oz
{
namespace client
{

class MD2Imago : public Imago
{
  public:

    static const float TURN_SMOOTHING_COEF;

    MD2*           md2;
    float          h;
    MD2::AnimState anim;

    static Pool<MD2Imago, 256> pool;

  protected:

    MD2Imago()
    {
      flags |= Imago::MD2MODEL_BIT;
    }

    virtual ~MD2Imago();

  public:

    static Imago* create( const Object* obj );

    void setAnim( Anim::Type anim );
    virtual void draw( const Imago* parent, int mask );

  OZ_STATIC_POOL_ALLOC( pool )

};

}
}
