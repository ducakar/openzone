/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/Render.hpp
 */

#pragma once

#include "matrix/Orbis.hpp"

#include "client/BSP.hpp"
#include "client/Terra.hpp"
#include "client/MD2.hpp"
#include "client/Imago.hpp"
#include "client/Context.hpp"

#include "client/ui/UI.hpp"

namespace oz
{
namespace client
{

class Render
{
  public:

    static const int DRAW_UI_BIT    = 0x01;
    static const int DRAW_ORBIS_BIT = 0x02;

  private:

    static const float WIDE_CULL_FACTOR;
    static const float CELL_WIDE_RADIUS;

    static const float NIGHT_FOG_COEFF;
    static const float NIGHT_FOG_DIST;
    static const float WATER_VISIBILITY;

    static const float WIND_FACTOR;
    static const float WIND_PHI_INC;

    struct ObjectEntry
    {
      float dist2;
      union
      {
        const Struct* str;
        const Object* obj;
      };

      ObjectEntry() = default;

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
    Vector<const Frag*>     frags;

    Vector<const Struct*>   waterStructs;

    float                   visibilityRange;
    float                   visibility;

    bool                    showBounds;
    bool                    showAim;

    bool                    isOffscreen;
    bool                    isDeferred;
    bool                    doPostprocess;

    float                   windPhi;

    float                   renderScale;
    int                     renderWidth;
    int                     renderHeight;

    uint                    mainFrame;
    uint                    depthBuffer;
    uint                    colourBuffer;
    uint                    normalBuffer;
    uint                    specularBuffer;

  public:

    SDL_Surface*            surface;

    uint                    prepareMillis;
    uint                    caelumMillis;
    uint                    terraMillis;
    uint                    structsMillis;
    uint                    objectsMillis;
    uint                    fragsMillis;
    uint                    miscMillis;
    uint                    postprocessMillis;
    uint                    uiMillis;
    uint                    syncMillis;

  private:

    void scheduleCell( int cellX, int cellY );
    void prepareDraw();
    void drawGeometry();
    void postprocess();

    void drawUI();
    void drawOrbis();

  public:

    void draw( int flags );
    void sync();
    void toggleFullscreen() const;

    void load();
    void unload();

    void init( bool isBuild = false );
    void free( bool isBuild = false );

};

extern Render render;

}
}
