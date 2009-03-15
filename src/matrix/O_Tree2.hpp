/*
 *  O_Tree2.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Object.hpp"

namespace oz
{

  struct O_Tree2 : Object
  {
    protected:

      void onDestroy();

    public:

      static const char *NAME;
      static const int TYPE;

      static Object *build( const Vec3 &p );

      explicit O_Tree2( const Vec3 &p );
      O_Tree2( float x, float y );

      void load( FILE *stream );
      void save( FILE *stream );

//       void createModel();
  };

}
