/*
 *  MD2WeaponModel.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "MD2Model.h"

namespace oz
{
namespace client
{

  struct MD2WeaponModel : public MD2Model
  {
    public:

      static Model* create( const Object* obj );

      virtual void draw( const Model* parent );

  };

}
}
