/*
 *  MD3StaticModel.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "Model.h"

namespace oz
{
namespace client
{

  struct MD3StaticModel : public Model
  {
    protected:

      uint list;

      virtual ~MD3StaticModel();

    public:

      static Model* create( const Object* obj );

      virtual void draw( const Model* parent );

  };

}
}
