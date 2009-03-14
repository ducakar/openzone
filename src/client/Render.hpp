/*
 *  Render.h
 *
 *  Graphics render engine
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/World.hpp"

#include "Font.hpp"
#include "BSP.hpp"
#include "Terrain.hpp"
#include "OBJ.hpp"
#include "MD2.hpp"
#include "Model.hpp"
#include "SparkGenRender.hpp"

namespace oz
{
namespace Client
{

  class Render
  {
    private:

      static const float RELEASED_CULL_FACTOR;
      static const float INCH;

      static const float BLACK[];
      static const float WHITE[];

      static const float GLOBAL_AMBIENT[];

      static const float NIGHT_FOG_COEFF;
      static const float NIGHT_FOG_DIST;

      Font         font;
      Terrain      terra;

      Vector<BSP*> bsps;
      Bitset       drawnStructures;

      Vector<uint> lists;

      Vector<MD2*> md2s;

      HashIndex<Model*, 1024> models;

      Vector<Structure*> structures;
      Vector<Object*>    objects;
      Vector<Object*>    blendedObjects;
      Vector<Object*>    waterObjects;
      Vector<Particle*>  particles;

      float perspectiveAngle;
      float perspectiveAspect;
      float perspectiveMin;
      float perspectiveMax;

      float particleRadius;

      bool  drawAABBs;
      bool  blendHeaven;
      bool  showAim;

      bool isSpirit;
      bool isUnderWater;

      void drawObject( Object *obj );
      void drawSparkGen( SparkGen *sparkGen );
      void scheduleSector( int sectorX, int sectorY );

    public:

      void init();
      void load();
      void free();

      void draw();
  };

  extern Render render;

}
}
