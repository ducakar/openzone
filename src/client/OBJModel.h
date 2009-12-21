/*
 *  OBJModel.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Model.h"

namespace oz
{
namespace client
{

  class OBJModel : public Model
  {
    protected:

      uint list;

      virtual ~OBJModel();

    public:

      static Model *create( const Object *obj );

      virtual void draw( const Model *parent );

  };

}
}
