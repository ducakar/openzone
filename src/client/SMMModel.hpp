/*
 *  SMMModel.hpp
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

  class SMMModel : public Model
  {
    protected:

      SMM*  smm;
      float h;

      virtual ~SMMModel();

    public:

      static Pool<SMMModel, 1024> pool;

      static Model* create( const Object* obj );

      virtual void draw( const Model* parent );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
