/*
 *  FloraManager.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

namespace oz
{

  class FloraManager
  {
    private:

      // plants/m2
      static const float DENSITY = 0.04;
      // dim * SPACING
      static const float SPACING = 8.0f;
      // plants/m2/s
      static const float GROWTH =  0.0001;

      Vector<int> plants;

      int number;
      int growth;

      void addTree( float x, float y );

    public:

      void seed();
      void clear();
      void update();

  };

  extern FloraManager floraManager;

}
