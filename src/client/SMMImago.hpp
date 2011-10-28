/*
 *  SMMImago.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/Imago.hpp"
#include "client/SMM.hpp"

namespace oz
{
namespace client
{

  class SMMImago : public Imago
  {
    protected:

      SMM* smm;

      virtual ~SMMImago();

    public:

      static Pool<SMMImago, 1024> pool;

      static Imago* create( const Object* obj );

      virtual void draw( const Imago* parent, int mask );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
