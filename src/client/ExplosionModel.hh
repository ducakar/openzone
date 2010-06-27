/*
 *  ExplosionModel.hh
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

#include "client/Model.hh"

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

      static Pool<ExplosionModel, 0, 256> pool;

      static Model* create( const Object* obj );

      virtual void draw( const Model* parent );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
