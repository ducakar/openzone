/*
 *  ExplosionImago.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "client/Imago.hpp"
#include "client/SMM.hpp"

namespace oz
{
namespace client
{

class ExplosionImago : public Imago
{
  protected:

    static int modelId;

    SMM* smm;
    int  startMillis;

    virtual ~ExplosionImago();

  public:

    static Pool<ExplosionImago> pool;

    static Imago* create( const Object* obj );

    virtual void draw( const Imago* parent, int mask );

  OZ_STATIC_POOL_ALLOC( pool )

};

}
}
