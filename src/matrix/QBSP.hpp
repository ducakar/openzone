/*
 *  QBSP.hpp
 *
 *  Data structure used for prebuilding BSP from Quake3 BSP.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#ifdef OZ_TOOLS

#include "matrix/BSP.hpp"

namespace oz
{

  class QBSP : public Bounds
  {
    private:

      static const int MAX_BRUSHES = 256;

      static const int QBSP_SLICK_FLAG_BIT    = 0x00000002;
      static const int QBSP_LADDER_FLAG_BIT   = 0x00000008;
      static const int QBSP_GLASS_FLAG_BIT    = 0x00000020;
      static const int QBSP_NONSOLID_FLAG_BIT = 0x00004000;
      static const int QBSP_WATER_TYPE_BIT    = 0x00000020;

      static const float DEFAULT_LIFE;
      static const float DEFAULT_RESISTANCE;

      struct QBSPHeader
      {
        char id[4];
        int  version;
      };

      struct QBSPLump
      {
        enum Lumps
        {
          ENTITIES,
          TEXTURES,
          PLANES,
          NODES,
          LEAFS,
          LEAFFACES,
          LEAFBRUSHES,
          MODELS,
          BRUSHES,
          BRUSHSIDES,
          VERTICES,
          INDICES,
          SHADERS,
          FACES,
          LIGHTMAPS,
          LIGHTVOLUMES,
          VISUALDATA,
          MAX
        };

        int offset;
        int length;
      };

      struct QBSPTexture
      {
        char name[64];
        int  flags;
        int  type;
      };

      struct QBSPPlane
      {
        float normal[3];
        float distance;
      };

      struct QBSPNode
      {
        int plane;

        int front;
        int back;

        int bb[2][3];
      };

      struct QBSPLeaf
      {
        int cluster;
        int area;

        int bb[2][3];

        int firstFace;
        int nFaces;

        int firstBrush;
        int nBrushes;
      };

      struct QBSPModel
      {
        float bb[2][3];

        int firstFace;
        int nFaces;

        int firstBrush;
        int nBrushes;
      };

      struct QBSPBrush
      {
        int firstSide;
        int nSides;
        int texture;
      };

      struct QBSPBrushSide
      {
        int plane;
        int texture;
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

      struct Model : Bounds
      {
        Vec3   move;

        int    firstBrush;
        int    nBrushes;

        float  ratioInc;
        int    flags;

        int    type;

        float  margin;
        float  timeout;
      };

      String  name;

      float   life;
      float   resistance;

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

      bool includes( const Brush& brush, float maxDim ) const;

      void optimise();
      void check( bool isOptimised ) const;
      bool save( const char* path );

      QBSP( const char* name, const char* path );
      ~QBSP();

    public:

      // create ozBSP from a Quake 3 QBSP and optimise it
      static void prebuild( const char* name );

  };

}

#endif
