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

      oz::BSP  *bsp;

      uint baseList;
      uint *textures;
      uint *lightMaps;

      Bitset drawnFaces;
      Bitset visibleLeafs;
      Bitset hiddenFaces;

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
