/*
 *  D_MetalBarrel.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "DynObject.hpp"

namespace oz
{

  class D_MetalBarrel : public DynObject
  {
    protected:

      void onDestroy();
      void onFrictBegin();
      void onFrictEnd();

    public:

      static const char *NAME;
      static const int TYPE;

      static Object *build( const Vec3 &p );

      explicit D_MetalBarrel( const Vec3 &p = Vec3::zero() );

//       void createModel();
  };

}
