/*
 *  ExplosionModel.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/Model.hpp"
#include "client/SMM.hpp"

namespace oz
{
namespace client
{

  class ExplosionModel : public Model
  {
    protected:

      static int modelId;

      SMM* smm;
      int  startMillis;

      virtual ~ExplosionModel();

    public:

      static Pool<ExplosionModel> pool;

      static Model* create( const Object* obj );

      virtual void draw( const Model* parent, int mask );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
