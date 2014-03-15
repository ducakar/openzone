/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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

#include <client/common.hh>

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
  static const float OBJECT_VISIBILITY_COEF;
  static const float FRAG_VISIBILITY_RANGE2;
  static const float CELL_RADIUS;
  static const float EFFECTS_DISTANCE;

  static const float NIGHT_FOG_COEFF;
  static const float NIGHT_FOG_DIST;
  static const float WATER_VISIBILITY;
  static const float LAVA_VISIBILITY;

  static const float WIND_FACTOR;
  static const float WIND_PHI_INC;

  static const int   GLOW_MINIFICATION;

  static const Vec4  STRUCT_AABB;
  static const Vec4  ENTITY_AABB;
  static const Vec4  SOLID_AABB;
  static const Vec4  NONSOLID_AABB;

  struct DrawEntry;
  struct Effect;

  SBitset<Orbis::MAX_STRUCTS> drawnStructs;

  List<DrawEntry>             structs;
  List<DrawEntry>             objects;

  float                       visibilityRange;
  float                       visibility;

  bool                        showBounds;
  bool                        showAim;

  bool                        isOffscreen;

  float                       windPhi;

  int                         windowWidth;
  int                         windowHeight;
  int                         frameWidth;
  int                         frameHeight;
  float                       scale;
  uint                        scaleFilter;

  uint                        mainFrame;
#ifndef GL_ES_VERSION_2_0
  uint                        minGlowFrame;
#endif
  uint                        depthBuffer;
  uint                        colourBuffer;
#ifndef GL_ES_VERSION_2_0
  uint                        glowBuffer;
  uint                        minGlowBuffer;
#endif

  List<Effect>                effects;

  Thread                      effectsThread;

  Semaphore                   effectsMainSemaphore;
  Semaphore                   effectsAuxSemaphore;

  volatile bool               areEffectsAlive;

public:

  ulong64                     prepareMicros;
  ulong64                     caelumMicros;
  ulong64                     terraMicros;
  ulong64                     meshesMicros;
  ulong64                     miscMicros;
  ulong64                     postprocessMicros;
  ulong64                     uiMicros;
  ulong64                     swapMicros;

private:

  static void effectsMain( void* );

  void cellEffects( int cellX, int cellY );
  void effectsRun();

  void scheduleCell( int cellX, int cellY );
  void prepareDraw();
  void drawGeometry();

  void drawUI();
  void drawOrbis();
  void swap();

public:

  void update( int flags );
  void resize();

  void load();
  void unload();

  void init();
  void destroy();

#ifdef OZ_DYNAMICS
  void drawDyn();
  void loadDyn();
  void unloadDyn();
#endif

};

extern Render render;

}
}
