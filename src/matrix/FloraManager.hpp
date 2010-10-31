/*
 *  FloraManager.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "./stable.hpp"

namespace oz
{

  class FloraManager
  {
    private:

      // plants/m2
      static const float DENSITY;
      // dim * SPACING
      static const float SPACING;
      // plants/m2/s
      static const float GROWTH;

      Vector<int> plants;

      int number;
      int growth;

      void addTree( float x, float y );
      void addPlant( const char* type, float x, float y );

    public:

      void seed();
      void clear();
      void update();

  };

  extern FloraManager floraManager;

}
