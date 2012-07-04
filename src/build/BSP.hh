/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file build/BSP.hh
 */

#pragma once

#include "matrix/BSP.hh"

#include "build/Compiler.hh"

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
    static const float DEFAULT_MARGIN;

    static const int   BOUND_OBJECTS          = 100;

    static const float LIQUID_ALPHA;
    static const float LIQUID_SPECULAR;
    static const float GLASS_ALPHA;
    static const float GLASS_SPECULAR;

    /// Quake
    static const int   QBSP_SLICK_FLAG_BIT    = 0x00000002;
    static const int   QBSP_LADDER_FLAG_BIT   = 0x00000008;
    static const int   QBSP_NONSOLID_FLAG_BIT = 0x00004000;
    static const int   QBSP_LAVA_TYPE_BIT     = 0x00000008;
    static const int   QBSP_SEA_TYPE_BIT      = 0x00000010;
    static const int   QBSP_WATER_TYPE_BIT    = 0x00000020;
    static const int   QBSP_AIR_TYPE_BIT      = 0x00200000;
    static const int   QBSP_ALPHA_TYPE_BIT    = 0x20000000;

    static const int   QBSP_LIGHTMAP_DIM      = 128;
    static const int   QBSP_LIGHTMAP_BPP      = 3;
    static const int   QBSP_LIGHTMAP_SIZE     = QBSP_LIGHTMAP_DIM * QBSP_LIGHTMAP_DIM *
                                                QBSP_LIGHTMAP_BPP;

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

    /// Temporary structure for textures.
    struct Texture
    {
      String name;
      int    flags;
      int    type;
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

    /// Temporary structure for model.
    struct Model : Bounds
    {
      String title;

      Vec3   move;

      BSP*   bsp;

      int    firstBrush;
      int    nBrushes;

      matrix::Model::Type type;

      float  margin;
      float  timeout;
      float  ratioInc;

      int    target;
      int    key;

      String openSound;
      String closeSound;
      String frictSound;
    };

    /// Temporary structure for bound objects.
    struct BoundObject
    {
      String  clazz;
      Point   pos;
      Heading heading;
    };

    String                  name;
    String                  title;
    String                  description;

    float                   life;
    float                   resistance;

    int                     nTextures;
    int                     nPlanes;
    int                     nNodes;
    int                     nLeaves;
    int                     nLeafBrushes;
    int                     nModels;
    int                     nBrushes;
    int                     nBrushSides;
    int                     nVertices;
    int                     nIndices;
    int                     nFaces;

    Texture*                textures;
    Plane*                  planes;
    matrix::BSP::Node*      nodes;
    matrix::BSP::Leaf*      leaves;
    int*                    leafBrushes;
    Model*                  models;
    matrix::BSP::Brush*     brushes;
    int*                    brushSides;
    ModelFaces*             modelFaces;
    Vertex*                 vertices;
    int*                    indices;
    Face*                   faces;

    String                  fragPool;
    int                     nFrags;

    String                  demolishSound;

    Vector<BoundObject>     boundObjects;

    Vec4                    waterFogColour;
    Vec4                    lavaFogColour;

    void load();
    void optimise();
    void check() const;
    void saveMatrix();
    void saveClient();

    explicit BSP( const char* name );
    ~BSP();

  public:

    // create ozBSP from a Quake 3 QBSP and optimise it
    static void build( const char* name );

};

}
}
