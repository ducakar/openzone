/*
 *  MD2StaticModel.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Model.h"

namespace oz
{
namespace client
{

  class MD2StaticModel : public Model
  {
    private:



    public:

      static Model *create( const char *path );

  };

}
}
