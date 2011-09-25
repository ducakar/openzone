/*
 *  BSP.hpp
 *
 *  Data structure and loaders for Internal BSP, Quake3 BSP and OpenBSP formats
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/common.hpp"

namespace oz
{

  class BSP : public Bounds
  {
    public:

      static const int MAX_BRUSHES = 256;

      struct Node
      {
        int plane;

        int front;
        int back;
      };

      struct Leaf
      {
        int firstBrush;
        int nBrushes;
      };

      struct Brush
      {
        int firstSide;
        int nSides;

        int material;
      };

      struct Model : Bounds
      {
        enum Type
        {
          IGNORING,
          BLOCKING,
          CRUSHING,
          AUTO_DOOR
        };

        static const int AUTOMATIC_BIT = 0x00000001;
        static const int LUA_BIT       = 0x00000002;

        Vec3  move;

        BSP*  bsp;

        int   firstBrush;
        int   nBrushes;

        float ratioInc;
        int   flags;
        Type  type;

        float margin;
        float timeout;

        int   openSample;
        int   closeSample;
        int   frictSample;
      };

      int     id;
      float   life;

      int     nPlanes;
      int     nNodes;
      int     nLeaves;
      int     nLeafBrushes;
      int     nModels;
      int     nBrushes;
      int     nBrushSides;

      Plane*  planes;
      Node*   nodes;
      Leaf*   leaves;
      int*    leafBrushes;
      Model*  models;
      Brush*  brushes;
      int*    brushSides;

    private:

      bool loadOZBSP( const char* file );
      void freeOZBSP();

    public:

      explicit BSP( int id );
      ~BSP();

  };

}
