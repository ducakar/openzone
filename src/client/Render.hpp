/*
 *  Render.hpp
 *
 *  Graphics render engine
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Orbis.hpp"

#include "client/ui/UI.hpp"

#include "client/BSP.hpp"
#include "client/Terra.hpp"
#include "client/MD2.hpp"
#include "client/Model.hpp"
#include "client/Context.hpp"

namespace oz
{
namespace client
{

  class Render
  {
    public:

      static const int   DRAW_UI_BIT    = 0x01;
      static const int   DRAW_ORBIS_BIT = 0x02;

    private:

      static const float WIDE_CULL_FACTOR;
      static const float CELL_WIDE_RADIUS;

      static const float NIGHT_FOG_COEFF;
      static const float NIGHT_FOG_DIST;
      static const float WATER_VISIBILITY;

      struct ObjectEntry
      {
        float dist2;
        union
        {
          const Struct* str;
          const Object* obj;
        };

        OZ_ALWAYS_INLINE
        ObjectEntry()
        {}

        OZ_ALWAYS_INLINE
        explicit ObjectEntry( float dist2_, const Struct* str_ ) :
            dist2( dist2_ ), str( str_ )
        {}

        OZ_ALWAYS_INLINE
        explicit ObjectEntry( float dist2_, const Object* obj_ ) :
            dist2( dist2_ ), obj( obj_ )
        {}

        OZ_ALWAYS_INLINE
        bool operator < ( const ObjectEntry& oe ) const
        {
          return dist2 < oe.dist2;
        }
      };

      Bitset                  drawnStructs;

      Vector<ObjectEntry>     structs;
      Vector<ObjectEntry>     objects;
      Vector<const Particle*> particles;

      Vector<const Struct*>   waterStructs;

      float                   nearDist2;

      float                   dayVisibility;
      float                   nightVisibility;
      float                   waterDayVisibility;
      float                   waterNightVisibility;

      float                   particleRadius;
      bool                    showBounds;
      bool                    showAim;

      bool                    isDeferred;
      bool                    doPostprocess;

      float                   visibility;

      float                   windFactor;
      float                   windPhiInc;
      float                   windPhi;

      int                     renderWidth;
      int                     renderHeight;

      uint                    frameBuffer;
      uint                    depthBuffer;
      uint                    colourBuffer;
      uint                    normalBuffer;

#ifndef OZ_TOOLS
      void scheduleCell( int cellX, int cellY );
      void prepareDraw();
      void drawGeometry();
      void postprocess();

      void drawUI();
      void drawOrbis();
#endif

    public:

      SDL_Surface* surface;

#ifndef OZ_TOOLS
      void draw( int flags );
      void sync() const;
      void toggleFullscreen() const;

      void load();
      void unload();
#endif

      void init();
      void free();

  };

  extern Render render;

}
}
