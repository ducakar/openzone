/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

namespace oz::client
{

namespace ui
{

class SettingsMenu;

}

class Render
{
  friend class ui::SettingsMenu;

public:

  static constexpr int UI_BIT                   = 0x01;
  static constexpr int ORBIS_BIT                = 0x02;
  static constexpr int EFFECTS_BIT              = 0x04;

private:

  static constexpr float WIDE_CULL_FACTOR       = 6.0f;
  static constexpr float OBJECT_VISIBILITY_COEF = 0.004f;
  static constexpr float FRAG_VISIBILITY_RANGE2 = 150.0f*150.0f;
  static constexpr float CELL_RADIUS            = (Cell::SIZE / 2 +
                                                   Object::MAX_DIM * WIDE_CULL_FACTOR) * 1.41f;
  static constexpr float EFFECTS_DISTANCE       = 192.0f;

  static constexpr float NIGHT_FOG_COEFF        = 2.0f;
  static constexpr float NIGHT_FOG_DIST         = 0.3f;
  static constexpr float WATER_VISIBILITY       = 32.0f;
  static constexpr float LAVA_VISIBILITY        = 4.0f;

  static constexpr float WIND_FACTOR            = 0.0008f;
  static constexpr float WIND_PHI_INC           = 0.04f;

  static constexpr int   GLOW_MINIFICATION      = 4;

  static constexpr Vec4  STRUCT_AABB            = Vec4(0.20f, 0.50f, 1.00f, 1.00f);
  static constexpr Vec4  ENTITY_AABB            = Vec4(1.00f, 0.20f, 0.50f, 1.00f);
  static constexpr Vec4  SOLID_AABB             = Vec4(0.50f, 0.80f, 0.20f, 1.00f);
  static constexpr Vec4  NONSOLID_AABB          = Vec4(0.70f, 0.80f, 0.90f, 1.00f);

  struct DrawEntry;

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
  float                       frameScale;
  uint                        frameScaleFilter;

  uint                        mainFrame;
  uint                        minGlowFrame;
  uint                        depthBuffer;
  uint                        colourBuffer;
  uint                        glowBuffer;
  uint                        minGlowBuffer;

  Thread                      effectsThread;

  Semaphore                   effectsMainSemaphore;
  Semaphore                   effectsAuxSemaphore;

  Atomic<bool>                areEffectsAlive;

public:

  Duration                    prepareDuration;
  Duration                    caelumDuration;
  Duration                    terraDuration;
  Duration                    meshesDuration;
  Duration                    miscDuration;
  Duration                    postprocessDuration;
  Duration                    uiDuration;
  Duration                    swapDuration;

private:

  static void* effectsMain(void*);

  void cellEffects(int cellX, int cellY);
  void effectsRun();

  void scheduleCell(int cellX, int cellY);
  void prepareDraw();
  void drawGeometry();

  void drawUI();
  void drawOrbis();
  void swapBuffers();

public:

  void update(int flags);
  void resize();

  void load();
  void unload();

  void init();
  void destroy();

};

extern Render render;

}
