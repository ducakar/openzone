/*
 *  MD2Model.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Model.h"
#include "MD2.h"

namespace oz
{
namespace client
{

  class MD2Model : public Model
  {
    public:

      MD2            *md2;
      MD2::AnimState anim;

      virtual ~MD2Model();

    public:

      static Model *create( const Object *obj );

      void setAnim( int type );
      virtual void draw( const Model *parent );

  };

}
}
