/*
 *  BSP.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file build/BSP.hpp
 */

#include "stable.hpp"

#include "matrix/BSP.hpp"

#include "client/Mesh.hpp"

namespace oz
{
namespace build
{

/**
 * BSP builder.
 *
 * Reads Quake 3 BSP & configuration file and prebuilds both matrix (.ozBSP) and client (.ozcBSP)
 * BSPs. It also strips bounding shell and performs some BSP optimisation.
 */
class BSP : public Bounds
{
  private:

    static const float DEFAULT_SCALE;
    static const float DEFAULT_LIFE;
    static const float DEFAULT_RESISTANCE;

    /// Quake
    static const int QBSP_SLICK_FLAG_BIT    = 0x00000002;
    static const int QBSP_LADDER_FLAG_BIT   = 0x00000008;
    static const int QBSP_GLASS_FLAG_BIT    = 0x00000020;
    static const int QBSP_NONSOLID_FLAG_BIT = 0x00004000;
    static const int QBSP_WATER_TYPE_BIT    = 0x00000020;

    static const int QBSP_LIGHTMAP_DIM  = 128;
    static const int QBSP_LIGHTMAP_BPP  = 3;
    static const int QBSP_LIGHTMAP_SIZE = QBSP_LIGHTMAP_DIM * QBSP_LIGHTMAP_DIM * QBSP_LIGHTMAP_BPP;

    /// Data structure for Quake 3 BSP lump entry.
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

    /// Data structure for Quake 3 BSP texture entry.
    struct QBSPTexture
    {
      char name[64];
      int  flags;
      int  type;
    };

    /// Data structure for Quake 3 BSP plane entry.
    struct QBSPPlane
    {
      float normal[3];
      float distance;
    };

    /// Data structure for Quake 3 BSP node entry.
    struct QBSPNode
    {
      int plane;

      int front;
      int back;

      int bb[2][3];
    };

    /// Data structure for Quake 3 BSP leaf (node) entry.
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

    /// Data structure for Quake 3 BSP model entry.
    struct QBSPModel
    {
      float bb[2][3];

      int firstFace;
      int nFaces;

      int firstBrush;
      int nBrushes;
    };

    /// Data structure for Quake 3 BSP brush entry.
    struct QBSPBrush
    {
      int firstSide;
      int nSides;
      int texture;
    };

    /// Data structure for Quake 3 BSP bush side entry.
    struct QBSPBrushSide
    {
      int plane;
      int texture;
    };

    /// Vertex in Quake 3 BSP.
    struct QBSPVertex
    {
      float p[3];
      float texCoord[2];
      float lightmapCoord[2];
      float normal[3];
      char  colour[4];
    };

    /// Face in Quake 3 BSP.
    struct QBSPFace
    {
      int   texture;
      int   effect;
      int   type;

      int   firstVertex;
      int   nVertices;

      int   firstIndex;
      int   nIndices;

      int   lightmap;
      int   lightmapCorner[2];
      int   lightmapSize[2];

      float lightmapPos[3];
      float lightmapVecs[2][3];

      float normal[3];

      int   size[2];
    };

    // Temporary structure for textures.
    struct Texture
    {
      int id;
      int flags;
      int type;
    };

    /// Temporary structure to hold per-model face indices.
    struct ModelFaces
    {
      int firstFace;
      int nFaces;
    };

    /// Temporary structure for face.
    struct Face
    {
      int  texture;

      int  firstVertex;
      int  nVertices;

      int  firstIndex;
      int  nIndices;

      Vec3 normal;
    };

    String          name;

    float           life;
    float           resistance;

    int             nTextures;
    int             nPlanes;
    int             nNodes;
    int             nLeaves;
    int             nLeafBrushes;
    int             nModels;
    int             nBrushes;
    int             nBrushSides;
    int             nVertices;
    int             nIndices;
    int             nFaces;

    Texture*        textures;
    Plane*          planes;
    oz::BSP::Node*  nodes;
    oz::BSP::Leaf*  leaves;
    int*            leafBrushes;
    oz::BSP::Model* models;
    oz::BSP::Brush* brushes;
    int*            brushSides;
    ModelFaces*     modelFaces;
    client::Vertex* vertices;
    int*            indices;
    Face*           faces;

    bool includes( const oz::BSP::Brush& brush, float maxDim ) const;

    void load();
    void optimise();
    void check() const;
    void saveMatrix();
    void saveClient();

    BSP( const char* name );
    ~BSP();

  public:

    // create ozBSP from a Quake 3 QBSP and optimise it
    static void prebuild( const char* name );

};

}
}
