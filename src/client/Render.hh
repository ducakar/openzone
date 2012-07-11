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

#include "client/Imago.hh"

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
    static const float OBJECT_VISIBILITY_COEF;
    static const float FRAG_VISIBILITY_RANGE2;

    static const float NIGHT_FOG_COEFF;
    static const float NIGHT_FOG_DIST;
    static const float WATER_VISIBILITY;
    static const float LAVA_VISIBILITY;

    static const float WIND_FACTOR;
    static const float WIND_PHI_INC;

    static const Vec4  STRUCT_AABB;
    static const Vec4  ENTITY_AABB;
    static const Vec4  SOLID_AABB;
    static const Vec4  NONSOLID_AABB;

    Bitset              drawnStructs;

    List<const Struct*> structs;
    List<const Object*> objects;

    float               visibilityRange;
    float               visibility;

    int                 flags;

    bool                showBounds;
    bool                showAim;

    bool                isOffscreen;
    bool                doPostprocess;
    bool                isLowDetail;

    float               windPhi;

    int                 windowWidth;
    int                 windowHeight;
    int                 frameWidth;
    int                 frameHeight;
    float               scale;
    int                 scaleFilter;

    uint                mainFrame;
    uint                depthBuffer;
    uint                colourBuffer;
    uint                normalBuffer;
    uint                specularBuffer;

  public:

    ulong64             prepareMicros;
    ulong64             caelumMicros;
    ulong64             terraMicros;
    ulong64             meshesMicros;
    ulong64             postprocessMicros;
    ulong64             uiMicros;
    ulong64             swapMicros;

  private:

    void scheduleCell( int cellX, int cellY );
    void prepareDraw();
    void drawGeometry();

    void drawUI();
    void drawOrbis();

  public:

    void draw( int flags );
    void swap();

    void resize();

    void load();
    void unload();

    void init( bool isBuild = false );
    void free( bool isBuild = false );

};

extern Render render;

}
}
