/*
 *  Render.hpp
 *
 *  Graphics render engine
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Orbis.hpp"

#include "ui/UI.hpp"

#include "client/BSP.hpp"
#include "client/Terra.hpp"
#include "client/OBJ.hpp"
#include "client/MD2.hpp"
#include "client/Model.hpp"
#include "client/Context.hpp"

namespace oz
{
namespace client
{

  class Render
  {
    private:

      static const float WIDE_CULL_FACTOR;
      static const float CELL_WIDE_RADIUS;

      static const float NIGHT_FOG_COEFF;
      static const float NIGHT_FOG_DIST;
      static const float WATER_VISIBILITY;

      static const float STAR_SIZE;

      struct ObjectEntry
      {
        float distance;
        const Object* obj;

        explicit ObjectEntry( float distance_, const Object* obj_ ) :
            distance( distance_ ), obj( obj_ )
        {}

        // sort in reverse order (farest to nearest)
        bool operator < ( const ObjectEntry& be ) const
        {
          return distance > be.distance;
        }
      };

      Bitset                  drawnStructs;

      Vector<const Struct*>   structs;
      Vector<ObjectEntry>     objects;
      Vector<const Particle*> particles;

      Vector<const Struct*>   waterStructs;

      float                   dayVisibility;
      float                   nightVisibility;
      float                   waterDayVisibility;
      float                   waterNightVisibility;

      float                   particleRadius;
      bool                    showBounds;
      bool                    showAim;

      bool                    isUnderWater;
      float                   visibility;

      void scheduleCell( int cellX, int cellY );

      // render world
      void drawOrbis();
      // UI, swap buffers, cleanup models
      void drawCommon();

    public:

      void draw();

      void load();
      void unload();

      void init();
      void free();

  };

  extern Render render;

}
}
