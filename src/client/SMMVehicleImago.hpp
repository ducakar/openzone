/*
 *  SMMVehicleImago.hpp
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

  class SMMVehicleImago : public Imago
  {
    protected:

      SMM* smm;

    public:

      static Pool<SMMVehicleImago> pool;

      static Imago* create( const Object* obj );

      virtual ~SMMVehicleImago();

      virtual void draw( const Imago* parent, int mask );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
