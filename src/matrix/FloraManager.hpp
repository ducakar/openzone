/*
 *  FloraManager.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

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
