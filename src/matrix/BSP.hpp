/*
 *  BSP.hpp
 *
 *  Data structure and loaders for Internal BSP, Quake3 BSP and OpenBSP formats
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "matrix/common.hpp"

namespace oz
{

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

      struct Model : Bounds
      {
        int  firstFace;
        int  nFaces;

        int  firstBrush;
        int  nBrushes;
      };

      struct Entity
      {
        static const int AUTOMATIC_BIT = 0x00000001;
        static const int PUSHING_BIT   = 0x00000002;
        static const int CRUSHING_BIT  = 0x00000004;

        Vec3  startPos;
        Vec3  endPos;

        int   flags;

        int   model;

        float slideTime;
        float timeout;
      };

      struct Brush
      {
        int firstSide;
        int nSides;

        int material;
      };

      struct Vertex
      {
        Vec3  p;

        float texCoord[2];
        float lightmapCoord[2];
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

      String     name;
      float      maxDim;
      float      life;

      int        nPlanes;
      int        nNodes;
      int        nLeaves;
      int        nLeafFaces;
      int        nLeafBrushes;
      int        nModels;
      int        nEntities;
      int        nBrushes;
      int        nBrushSides;
      int        nTextures;
      int        nVertices;
      int        nIndices;
      int        nFaces;
      int        nLightmaps;

      Plane*     planes;
      Node*      nodes;
      Leaf*      leaves;
      int*       leafBrushes;
      int*       leafFaces;
      Model*     models;
      Entity*    entities;
      Brush*     brushes;
      int*       brushSides;
      int*       textures;
      Vertex*    vertices;
      int*       indices;
      Face*      faces;
      Lightmap*  lightmaps;

      VisualData visual;

    private:

#ifndef OZ_USE_PREBUILT
      bool includes( const Brush& brush ) const;

      bool loadQBSP( const char* fileName );
      void optimise();
      bool save( const char* fileName );
#endif

      bool loadOZBSP( const char* fileName );

    public:

      explicit BSP();
      ~BSP();

      bool load( const char* name );
      void free();

  };

}
