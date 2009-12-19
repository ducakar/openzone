/*
 *  Render.h
 *
 *  Graphics render engine
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "matrix/World.h"

#include "ui/UI.h"

#include "BSP.h"
#include "Terrain.h"
#include "OBJ.h"
#include "MD2.h"
#include "Model.h"

namespace oz
{
namespace client
{

  class Render
  {
    private:

      static const float RELEASED_CULL_FACTOR = 8.0f;

      static const float NIGHT_FOG_COEFF;
      static const float NIGHT_FOG_DIST;
      static const float WATER_VISIBILITY;

      static const float STAR_SIZE;

      // cleanup interval (remove unused models)
      static const int   CLEAR_INTERVAL = 303 * 1000;

      struct ObjectEntry
      {
        float distance;
        const Object *obj;

        ObjectEntry() {}
        ObjectEntry( float distance_, const Object *obj_ ) : distance( distance_ ), obj( obj_ ) {}

        // sort in reverse order (farest to nearest)
        bool operator < ( const ObjectEntry &be ) const
        {
          return distance > be.distance;
        }
      };

      Terrain                  terra;
      Vector<BSP*>             bsps;
      Bitset                   drawnStructures;

      int                      clearCount;

      Vector<const Structure*> structures;
      Vector<ObjectEntry>      objects;
      Vector<ObjectEntry>      delayedObjects;
      Vector<const Particle*>  particles;

      Vector<const Structure*> waterStructures;
      HashIndex<Model*, 1021>  models;

      int                      screenX;
      int                      screenY;

      double                   perspectiveAngle;
      double                   perspectiveAspect;
      double                   perspectiveMin;
      double                   perspectiveMax;

      float                    dayVisibility;
      float                    nightVisibility;
      float                    waterDayVisibility;
      float                    waterNightVisibility;

      float                    particleRadius;
      bool                     drawAABBs;
      bool                     showAim;

      bool                     isUnderWater;
      bool                     wasUnderWater;
      float                    visibility;
      int                      taggedObj;

      void scheduleCell( int cellX, int cellY );

    public:

      bool doScreenshot;

      void drawModel( const Object *obj );
      void drawMountedModel( const Object *obj );

      void sync();
      void update();

      void init();
      void free();

      void load();
      void unload();
  };

  extern Render render;

}
}
