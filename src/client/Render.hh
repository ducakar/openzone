/*
 *  Render.hh
 *
 *  Graphics render engine
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

#include "matrix/World.hh"

#include "ui/UI.hh"

#include "client/BSP.hh"
#include "client/Terrain.hh"
#include "client/OBJ.hh"
#include "client/MD2.hh"
#include "client/Model.hh"
#include "client/Context.hh"

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

      // cleanup interval (remove unused models)
      static const int   CLEAR_INTERVAL = 303 * 1000;

      struct ObjectEntry
      {
        float distance;
        const Object* obj;

        ObjectEntry( float distance_, const Object* obj_ ) : distance( distance_ ), obj( obj_ ) {}

        // sort in reverse order (farest to nearest)
        bool operator < ( const ObjectEntry& be ) const
        {
          return distance > be.distance;
        }
      };

      Vector<BSP*>             bsps;
      Bitset                   drawnStructures;

      int                      clearCount;

      Vector<const Structure*> structures;
      Vector<ObjectEntry>      objects;
      Vector<ObjectEntry>      delayedObjects;
      Vector<const Particle*>  particles;

      Vector<const Structure*> waterStructures;
      HashIndex<Model*, 4093>  models;

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

      void scheduleCell( int cellX, int cellY );

      // render world
      void drawWorld();
      // UI, swap buffers, cleanup models
      void drawCommon();

    public:

      bool doScreenshot;

      void drawModel( const Object* obj, const Model* parent )
      {
        Model* const* value = models.find( obj->index );
        if( value == null ) {
          value = models.add( obj->index, context.createModel( obj ) );
        }
        Model* model = *value;

        model->flags |= Model::UPDATED_BIT;
        model->draw( parent );
      }

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
