/*
 *  BSP.h
 *
 *  BSP level rendering class
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/Structure.h"

#define BSP_GAMMA_CORR        0.5

namespace oz
{
namespace client
{

  class BSP
  {
    private:

      static const float WATER_TEX_STRETCH;
      static const float WATER_TEX_BIAS;
      static const float WATER_ALPHA;

      // water
      static float waterPhi;
      static float waterAlpha1;
      static float waterAlpha2;

      oz::BSP *bsp;

      uint baseList;
      uint *textures;
      uint *lightMaps;

      Bitset drawnFaces;
      Bitset visibleLeafs;
      Bitset hiddenFaces;

      static Bounds rotateBounds( const Bounds &bounds, Structure::Rotation rotation )
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

      const oz::BSP::Leaf *getLeaf( const Vec3 &p ) const;
      bool isInWaterBrush( const Vec3 &p, const oz::BSP::Leaf *leaf ) const;

      void compileFace( int faceIndex ) const;
      void drawFace( int faceIndex ) const;
      void drawNode( int index );

    public:

      BSP() {};
      ~BSP();

      explicit BSP( oz::BSP *bsp );

      void init( oz::BSP *bsp );
      bool draw( const Structure *str );
      uint genList();

      static void beginRender();
      static void endRender();

      void free();

  };

}
}
