/*
 *  BSP.hpp
 *
 *  Data structure and loaders for Internal BSP, Quake3 BSP and OpenBSP formats
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "bv.hpp"

namespace oz
{

  struct BSP : Bounds
  {

    static const int LIGHTMAP_DIM = 128;
    static const int LIGHTMAP_BPP = 3;
    static const int LIGHTMAP_SIZE = LIGHTMAP_DIM * LIGHTMAP_DIM * LIGHTMAP_BPP;

    // collidable flag for simplex side
    static const int COLLIDABLE_BIT = 0x00000001;

    // 1 unit in BSP map = 1 cm (for BSPs made for this engine)
    static const float BSP_SCALE;
    // 1 unit in BSP map = 1 inch (for BSPs made for quake)
    static const float QBSP_SCALE;

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
      int   cluster;

      int   firstFace;
      int   nFaces;

      int   firstSimplex;
      int   nSimplexes;
    };

    struct Simplex
    {
      int firstSide;
      int nSides;
      int flags;
    };

    struct Vertex
    {
      Vec3  p;
      float texCoord[2];
      float lightmapCoord[2];
    };

    struct Face
    {
      int  texture;

      int  firstVertex;
      int  nVertices;

      int  firstIndex;
      int  nIndices;

      int  lightmap;

      Vec3 normal;
    };

    struct Lightmap
    {
      char bits[LIGHTMAP_SIZE];
    };

    struct VisualData
    {
      int    nClusters;
      int    clusterLength;
      Bitset *bitsets;

      VisualData();
      ~VisualData();
    };

    float         maxDim;

    int           nTextures;
    int           nLeafs;
    int           nFaces;
    int           nLightmaps;

    int           *textures;
    Plane         *planes;

    Node          *nodes;
    Leaf          *leafs;
    int           *leafFaces;
    int           *leafSimplexes;

    Simplex       *simplices;
    int           *simplexSides;

    Vertex        *vertices;
    int           *indices;
    Face          *faces;
    Lightmap      *lightmaps;

    VisualData    visual;

    BSP();
    ~BSP();

    void loadQBSP( const char *fileName, float scale = QBSP_SCALE, float maxDim = Math::INF );

    void strip();
    void free();
  };

}
