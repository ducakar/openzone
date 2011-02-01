/*
 *  ExplosionModel.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/Model.hpp"

class GLUquadric;

namespace oz
{
namespace client
{

  class ExplosionModel : public Model
  {
    protected:

      uint        texId;
      GLUquadric* quadric;
      int         startMillis;

      virtual ~ExplosionModel();

    public:

      static Pool<ExplosionModel, 256> pool;

      static Model* create( const Object* obj );

      virtual void draw( const Model* parent );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
