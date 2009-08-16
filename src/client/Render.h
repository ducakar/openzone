/*
 *  Render.h
 *
 *  Graphics render engine
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/World.h"

#include "ui/ui.h"

#include "BSP.h"
#include "Terrain.h"
#include "OBJ.h"
#include "MD2.h"
#include "Model.h"
#include "SparkGenRender.h"

namespace oz
{
namespace client
{

  class Render
  {
    private:

      static const float RELEASED_CULL_FACTOR;

      static const float BLACK[];
      static const float WHITE[];

      static const float GLOBAL_AMBIENT[];

      static const float NIGHT_FOG_COEFF;
      static const float NIGHT_FOG_DIST;

      static const float WATER_COLOR[];
      static const float WATER_VISIBILITY;

      static const int   DELAYED_LISTS_MAX = 256;

      // cleanup interval (remove unused models)
      static const int   CLEAR_INTERVAL = 60 * 50;

      struct DelayedList
      {
        float distance;
        uint  list;
      };

    private:

      Terrain                 terra;
      Vector<BSP*>            bsps;
      Bitset                  drawnStructures;

      HashIndex<Model*, 1021> models;
      int                     clearCount;

      Vector<Structure*>      structures;
      Vector<Object*>         objects;
      Vector<Object*>         blendedObjects;
      Vector<Particle*>       particles;

      Vector<Structure*>      waterStructures;

      int                     screenX;
      int                     screenY;

      double                  perspectiveAngle;
      double                  perspectiveAspect;
      double                  perspectiveMin;
      double                  perspectiveMax;

      float                   particleRadius;

      bool                    drawAABBs;
      bool                    blendHeaven;
      bool                    showAim;

      bool                    isSpirit;
      bool                    isUnderWater;
      bool                    wasUnderWater;

      ui::Area                root;

      void drawObject( Object *obj );
      void drawSparkGen( SparkGen *sparkGen );
      void scheduleSector( int sectorX, int sectorY );

    public:

      bool                    doScreenshot;

      void init();
      void load();
      void free();

      void sync();
      void update();
  };

  extern Render render;

}
}
