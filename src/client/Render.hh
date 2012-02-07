/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 */

/**
 * @file client/Render.hh
 */

#pragma once

#include "matrix/Orbis.hh"

#include "client/BSP.hh"
#include "client/Terra.hh"
#include "client/MD2.hh"
#include "client/Imago.hh"
#include "client/Context.hh"

#include "client/ui/UI.hh"

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

    struct ModelEntry
    {
      float dist2;
      union
      {
        const Struct* str;
        const Object* obj;
        const Frag*   frag;
      };

      ModelEntry() = default;

      OZ_ALWAYS_INLINE
      explicit ModelEntry( float dist2_, const Struct* str_ ) :
        dist2( dist2_ ), str( str_ )
      {}

      OZ_ALWAYS_INLINE
      explicit ModelEntry( float dist2_, const Object* obj_ ) :
        dist2( dist2_ ), obj( obj_ )
      {}

      OZ_ALWAYS_INLINE
      explicit ModelEntry( float dist2_, const Frag* frag_ ) :
        dist2( dist2_ ), frag( frag_ )
      {}

      OZ_ALWAYS_INLINE
      bool operator < ( const ModelEntry& me ) const
      {
        return dist2 < me.dist2;
      }
    };

    Bitset                drawnStructs;

    Vector<ModelEntry>    structs;
    Vector<ModelEntry>    objects;
    Vector<ModelEntry>    frags;

    Vector<const Struct*> waterStructs;

    float                 visibilityRange;
    float                 visibility;

    bool                  showBounds;
    bool                  showAim;

    bool                  isOffscreen;
    bool                  isDeferred;
    bool                  doPostprocess;
    bool                  isLowDetail;

    float                 windPhi;

    float                 renderScale;
    int                   renderWidth;
    int                   renderHeight;

    uint                  offscreenFilter;
    uint                  mainFrame;
    uint                  depthBuffer;
    uint                  colourBuffer;
    uint                  normalBuffer;
    uint                  specularBuffer;

  public:

    SDL_Surface*          surface;

    ulong64               prepareMicros;
    ulong64               setupMicros;
    ulong64               caelumMicros;
    ulong64               terraMicros;
    ulong64               structsMicros;
    ulong64               objectsMicros;
    ulong64               fragsMicros;
    ulong64               miscMicros;
    ulong64               postprocessMicros;
    ulong64               uiMicros;
    ulong64               swapMicros;

  private:

    void scheduleCell( int cellX, int cellY );
    void prepareDraw();
    void drawGeometry();

    void drawUI();
    void drawOrbis();

  public:

    bool toggleFullscreen() const;

    void draw( int flags );
    void swap();

    void load();
    void unload();

    void init( bool isBuild = false );
    void free( bool isBuild = false );

};

extern Render render;

}
}
