/*
 *  BSP.hpp
 *
 *  BSP level rendering class
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Struct.hpp"
#include "matrix/BSP.hpp"

namespace oz
{
namespace client
{

  class BSP : public Bounds
  {
    private:

      static const int LIGHTMAP_DIM  = 128;
      static const int LIGHTMAP_BPP  = 3;
      static const int LIGHTMAP_SIZE = LIGHTMAP_DIM * LIGHTMAP_DIM * LIGHTMAP_BPP;

    public:

      static const int DRAW_WATER     = 1;
      static const int IN_WATER_BRUSH = 2;

    private:

      struct EntityModel
      {
        int firstFace;
        int nFaces;
      };

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
        int firstFace;
        int nFaces;

        int cluster;
      };

      struct Vertex
      {
        Point3   p;
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

        explicit VisualData();
        ~VisualData();
      };

      static const Struct* str;
      static Point3  camPos;
      static int     waterFlags;

      const oz::BSP* bsp;
      String         name;

      int            nTextures;
      int            nPlanes;
      int            nNodes;
      int            nLeaves;
      int            nEntityModels;
      int            nVertices;
      int            nIndices;
      int            nLeafFaces;
      int            nFaces;
      int            nLightmaps;

      int*           textures;
      Plane*         planes;
      Node*          nodes;
      Leaf*          leaves;
      EntityModel*   entityModels;
      Vertex*        vertices;
      int*           indices;
      int*           leafFaces;
      Face*          faces;
      Lightmap*      lightmaps;

      VisualData     visual;

      uint           baseList;
      uint*          texIds;
      uint*          lightmapIds;

      Bitset         drawnFaces;
      Bitset         visibleLeafs;
      Bitset         hiddenFaces;

      static Bounds rotateBounds( const Bounds& bounds, Struct::Rotation rotation );

      int  getLeaf() const;
      bool isInWater() const;

      void drawFace( const Face* face ) const;
      void drawFaceWater( const Face* face ) const;
      // This function  _should_ draw a BSP without depth testing if only OpenGL supported some kind
      // of depth func that would draw pixel only if it hasn't been drawn yet.
      void drawNode( int nodeIndex );
      void drawNodeWater( int nodeIndex );

      bool loadOZCBSP( const char* fileName );
      void freeOZCBSP();

      bool loadQBSP( const char* fileName );
      void freeQBSP( const char* name );
      bool save( const char* file );

      // used internally by prebuild
      explicit BSP();

    public:

      bool isUpdated;

      // create ozcBSP from a Quake 3 QBSP (matrix BSP must be loaded)
      static void prebuild( const char* name );

      explicit BSP( int bspIndex );
      ~BSP();

      void init( oz::BSP* bsp );

      int  draw( const Struct* str );
      void drawWater( const Struct* str );
      int  fullDraw( const Struct* str );
      void fullDrawWater( const Struct* str );

      uint genList();

      static void beginRender();
      static void endRender();

  };

}
}
