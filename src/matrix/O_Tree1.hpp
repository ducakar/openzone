/*
 *  O_Tree1.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Object.hpp"

namespace oz
{

  struct O_Tree1 : Object
  {
    protected:

      void onDestroy();

    public:

      static const char *NAME;
      static const int TYPE;

      static Object *build( const Vec3 &p );

      explicit O_Tree1( const Vec3 &p );
      O_Tree1( float x, float y );

      void load( FILE *stream );
      void save( FILE *stream );

//       void createModel();
  };

}
