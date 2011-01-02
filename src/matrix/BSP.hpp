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

    int   firstFace;
    int   nFaces;

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

      static const int LIGHTMAP_DIM = 128;
      static const int LIGHTMAP_BPP = 3;
      static const int LIGHTMAP_SIZE = LIGHTMAP_DIM * LIGHTMAP_DIM * LIGHTMAP_BPP;

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

      struct Leaf : Bounds
      {
        int firstBrush;
        int nBrushes;

        int firstFace;
        int nFaces;

        int cluster;
      };

      struct Brush
      {
        int firstSide;
        int nSides;

        int material;
      };

      struct Vertex
      {
        Vec3     p;
        TexCoord texCoord;
        TexCoord lightmapCoord;
      };

      struct Face
      {
        Vec3 normal;

        int  texture;
        int  lightmap;
        int  material;

        int  firstVertex;
        int  nVertices;

        int  firstIndex;
        int  nIndices;
      };

      struct Lightmap
      {
        char bits[LIGHTMAP_SIZE];
      };

      struct VisualData
      {
        int     nClusters;
        int     clusterLength;
        Bitset* bitsets;

        explicit VisualData() : bitsets( null ) {}

        ~VisualData()
        {
          if( bitsets != null ) {
            delete[] bitsets;
          }
        }
      };

      String       name;
      float        maxDim;
      float        life;

      int          nPlanes;
      int          nNodes;
      int          nLeaves;
      int          nLeafBrushes;
      int          nLeafFaces;
      int          nEntityClasses;
      int          nBrushes;
      int          nBrushSides;
      int          nTextures;
      int          nVertices;
      int          nIndices;
      int          nFaces;
      int          nLightmaps;

      Plane*       planes;
      Node*        nodes;
      Leaf*        leaves;
      int*         leafBrushes;
      int*         leafFaces;
      EntityClass* entityClasses;
      Brush*       brushes;
      int*         brushSides;
      int*         textures;
      Vertex*      vertices;
      int*         indices;
      Face*        faces;
      Lightmap*    lightmaps;

      VisualData   visual;

    private:

      bool includes( const Brush& brush ) const;

      bool loadQBSP( const char* file );
      // free BSP loaded from a QBSP
      void freeQBSP();

      void optimise();

      bool save( const char* file );

      bool loadOZBSP( const char* file );

      // free prebuilt ozBSP structure
      void freeOZBSP();

      // used internally by prebuild
      explicit BSP();

    public:

      // create BSP from a Quake 3 QBSP and optimise it
      static void prebuild( const char* name );

      // create BSP from a prebuilt ozBSP
      explicit BSP( const char* name );

      ~BSP();

  };

}
