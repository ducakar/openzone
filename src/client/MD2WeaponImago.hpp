/*
 *  MD2WeaponImago.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "client/MD2Imago.hpp"

namespace oz
{
namespace client
{

class MD2WeaponImago : public MD2Imago
{
  public:

    static Pool<MD2WeaponImago> pool;

    static Imago* create( const Object* obj );

    virtual void draw( const Imago* parent, int mask );

  OZ_STATIC_POOL_ALLOC( pool )

};

}
}
