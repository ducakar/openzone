/*
 *  BSP.hpp
 *
 *  Data structure and loaders for Internal BSP, Quake3 BSP and OpenBSP formats
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/common.hpp"

namespace oz
{

  class BSP;

  struct EntityClass : Bounds
  {
    enum Mode
    {
      IGNORING = 0,
      BLOCKING = 1,
      PUSHING  = 2,
      CRUSHING = 3
    };

    static const int AUTOMATIC_BIT = 0x00000001;
    static const int LUA_BIT       = 0x00000002;

    BSP*  bsp;

    int   firstBrush;
    int   nBrushes;

    Vec3  move;
    float ratioInc;
    int   flags;
    Mode  mode;

    float margin;
    float slideTime;
    float timeout;
  };

  class BSP : public Bounds
  {
    public:

      static const int MAX_BRUSHES = 256;

      struct Plane
      {
        Vec3  normal;
        float distance;
      };

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

      String       name;
      float        life;

      int          nPlanes;
      int          nNodes;
      int          nLeaves;
      int          nLeafBrushes;
      int          nEntityClasses;
      int          nBrushes;
      int          nBrushSides;

      Plane*       planes;
      Node*        nodes;
      Leaf*        leaves;
      int*         leafBrushes;
      EntityClass* entityClasses;
      Brush*       brushes;
      int*         brushSides;

    private:

      bool includes( const Brush& brush, float maxDim ) const;

      bool loadOZBSP( const char* fileName );
      void freeOZBSP();

      // prebuild
      bool loadQBSP( const char* fileName );
      void freeQBSP();
      void optimise();
      bool save( const char* fileName );

      // used internally by prebuild
      explicit BSP();

    public:

      // create ozBSP from a Quake 3 QBSP and optimise it
      static void prebuild( const char* name );

      // create BSP from a prebuilt ozBSP
      explicit BSP( const char* name );
      ~BSP();

  };

}
