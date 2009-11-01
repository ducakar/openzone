/*
 *  BSP.h
 *
 *  BSP level rendering class
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/Structure.h"
#include "matrix/BSP.h"

namespace oz
{
namespace client
{

  class BSP
  {
    public:

      static const int DRAW_WATER     = 1;
      static const int IN_WATER_BRUSH = 2;

    private:

      static const float GAMMA_CORR;

      static const Structure *str;
      static Vec3 camPos;

      oz::BSP *bsp;

      uint baseList;
      uint *textures;
      uint *lightMaps;

      Bitset drawnFaces;
      Bitset visibleLeafs;
      Bitset hiddenFaces;

      static int waterFlags;
      static Vector<const oz::BSP::Face*> waterFaces;

      static Bounds rotateBounds( const Bounds &bounds, Structure::Rotation rotation );

      int  getLeaf() const;
      void checkInWaterBrush( const oz::BSP::Leaf *leaf ) const;

      void drawFace( const oz::BSP::Face *face ) const;
      void drawFaceWater( const oz::BSP::Face *face ) const;
      // This function  _should_ draw a BSP without depth testing if only OpenGL supported some kind
      // of depth func that would draw pixel only if it hasn't been drawn yet.
      void drawNode( int nodeIndex );
      void drawNodeWater( int nodeIndex );

    public:

      explicit BSP( oz::BSP *bsp );
      ~BSP();

      void init( oz::BSP *bsp );
      int  draw( const Structure *str );
      void drawWater( const Structure *str );
      static void drawInWater();
      uint genList();

      static void beginRender();
      static void endRender();

  };

}
}
