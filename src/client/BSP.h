/*
 *  BSP.h
 *
 *  BSP level rendering class
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/Structure.h"

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
      static const float WATER_TEX_STRETCH;
      static const float WATER_TEX_BIAS;
      static const float WATER_ALPHA;

      // water
      static float waterPhi;
      static float waterAlpha1;
      static float waterAlpha2;

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
      uint genList();

      static void beginRender();
      static void endRender();

  };

  inline Bounds BSP::rotateBounds( const Bounds &bounds, Structure::Rotation rotation )
  {
    Bounds rotatedBounds;

    switch( rotation ) {
      case Structure::R0: {
        rotatedBounds = bounds;
        break;
      }
      case Structure::R90: {
        rotatedBounds.mins.x = -bounds.maxs.y;
        rotatedBounds.mins.y =  bounds.mins.x;
        rotatedBounds.mins.z =  bounds.mins.z;

        rotatedBounds.maxs.x = -bounds.mins.y;
        rotatedBounds.maxs.y =  bounds.maxs.x;
        rotatedBounds.maxs.z =  bounds.maxs.z;
        break;
      }
      case Structure::R180: {
        rotatedBounds.mins.x = -bounds.maxs.x;
        rotatedBounds.mins.y = -bounds.maxs.y;
        rotatedBounds.mins.z =  bounds.mins.z;

        rotatedBounds.maxs.x = -bounds.mins.x;
        rotatedBounds.maxs.y = -bounds.mins.y;
        rotatedBounds.maxs.z =  bounds.maxs.z;
        break;
      }
      default:
        case Structure::R270: {
          rotatedBounds.mins.x =  bounds.mins.y;
          rotatedBounds.mins.y = -bounds.maxs.x;
          rotatedBounds.mins.z =  bounds.mins.z;

          rotatedBounds.maxs.x =  bounds.maxs.y;
          rotatedBounds.maxs.y = -bounds.mins.x;
          rotatedBounds.maxs.z =  bounds.maxs.z;
          break;
        }
    }
    return bounds;
  }

}
}
