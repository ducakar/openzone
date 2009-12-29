/*
 *  ExplosionModel.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "Model.h"

class GLUquadric;

namespace oz
{
namespace client
{

  struct ExplosionModel : public Model
  {
    protected:

      uint        texId;
      GLUquadric* quadric;
      int         startMillis;

      virtual ~ExplosionModel();

    public:

      static Model* create( const Object* obj );

      virtual void draw( const Model* parent );

  };

}
}
