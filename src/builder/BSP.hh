/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
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
 * @file builder/BSP.hh
 */

#pragma once

#include <matrix/BSP.hh>

#include <builder/Compiler.hh>

namespace oz::builder
{

/**
 * BSP builder.
 *
 * Reads Quake 3 BSP & configuration file and prebuilds both matrix (.ozBSP) and client (.ozcModel)
 * BSPs. It also strips bounding shell and performs some BSP optimisation.
 */
class BSP : public Bounds
{
private:

  static constexpr float DEFAULT_SCALE          = 0.01f;
  static constexpr float DEFAULT_LIFE           = 10000.0f;
  static constexpr float DEFAULT_RESISTANCE     = 400.0f;
  static constexpr float DEFAULT_MARGIN         = 0.1f;

  static constexpr float LIQUID_ALPHA           = 0.75f;
  static constexpr float LIQUID_SPECULAR        = 0.50f;
  static constexpr float GLASS_ALPHA            = 0.15f;
  static constexpr float GLASS_SPECULAR         = 2.00f;

  // Quake
  static constexpr int   QBSP_SLICK_FLAG_BIT    = 0x00000002;
  static constexpr int   QBSP_LADDER_FLAG_BIT   = 0x00000008;
  static constexpr int   QBSP_NONSOLID_FLAG_BIT = 0x00004000;
  static constexpr int   QBSP_LAVA_TYPE_BIT     = 0x00000008;
  static constexpr int   QBSP_SEA_TYPE_BIT      = 0x00000010;
  static constexpr int   QBSP_WATER_TYPE_BIT    = 0x00000020;
  static constexpr int   QBSP_AIR_TYPE_BIT      = 0x00200000;
  static constexpr int   QBSP_ALPHA_TYPE_BIT    = 0x20000000;

  static constexpr int   QBSP_LIGHTMAP_DIM      = 128;
  static constexpr int   QBSP_LIGHTMAP_BPP      = 3;
  static constexpr int   QBSP_LIGHTMAP_SIZE     = QBSP_LIGHTMAP_DIM * QBSP_LIGHTMAP_DIM *
                                                  QBSP_LIGHTMAP_BPP;

  // Data structure for Quake 3 BSP lump entry.
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

  // Data structure for Quake 3 BSP texture entry.
  struct QBSPTexture
  {
    char name[64];
    int  flags;
    int  type;
  };

  // Data structure for Quake 3 BSP plane entry.
  struct QBSPPlane
  {
    float normal[3];
    float distance;
  };

  // Data structure for Quake 3 BSP node entry.
  struct QBSPNode
  {
    int plane;

    int front;
    int back;

    int bb[2][3];
  };

  // Data structure for Quake 3 BSP leaf (node) entry.
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

  // Data structure for Quake 3 BSP model entry.
  struct QBSPModel
  {
    float bb[2][3];

    int   firstFace;
    int   nFaces;

    int   firstBrush;
    int   nBrushes;
  };

  // Data structure for Quake 3 BSP brush entry.
  struct QBSPBrush
  {
    int firstSide;
    int nSides;
    int texture;
  };

  // Data structure for Quake 3 BSP bush side entry.
  struct QBSPBrushSide
  {
    int plane;
    int texture;
  };

  // Vertex in Quake 3 BSP.
  struct QBSPVertex
  {
    float pos[3];
    float texCoord[2];
    float lightmapCoord[2];
    float normal[3];
    char  colour[4];
  };

  // Face in Quake 3 BSP.
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
    String name;
    int    flags;
    int    type;
  };

  // Temporary structure to hold per-model face indices.
  struct ModelFaces
  {
    int firstFace;
    int nFaces;
  };

  // Temporary structure for face.
  struct Face
  {
    int  texture;

    int  firstVertex;
    int  nVertices;

    int  firstIndex;
    int  nIndices;

    Vec3 normal;
  };

  // Temporary structure for BSP entity (EntityClass).
  struct Model : Bounds
  {
    int               firstBrush;
    int               nBrushes;

    String            title;

    EntityClass::Type type;
    int               flags;

    float             closeTimeout;
    float             openTimeout;

    Vec3              moveDir;
    float             moveLength;
    float             moveStep;

    float             margin;

    String            openSound;
    String            closeSound;
    String            frictSound;

    int               target;
    int               key;

    String            modelName;
    Mat4              modelTransf;
  };

  // Temporary structure for bound objects.
  struct BoundObject
  {
    String  clazz;
    Point   pos;
    Heading heading;
  };

  String               name;
  String               title;
  String               description;

  float                life;
  float                resistance;

  List<Texture>        textures;
  List<Plane>          planes;
  List<oz::BSP::Node>  nodes;
  List<oz::BSP::Leaf>  leaves;
  List<int>            leafBrushes;
  List<oz::BSP::Brush> brushes;
  List<int>            brushSides;
  List<Model>          models;
  List<ModelFaces>     modelFaces;
  List<QBSPVertex>     vertices;
  List<int>            indices;
  List<Face>           faces;

  List<int>            leafClusters;
  Bitset               clusters;
  int                  nClusters;
  int                  nClusterBytes;

  String               fragPool;
  int                  nFrags;

  String               demolishSound;

  float                groundOffset;

  List<BoundObject>    boundObjects;

  Vec4                 waterFogColour;
  Vec4                 lavaFogColour;

  void load();
  void optimise();
  void check() const;
  void saveMatrix();
  void saveClient();

public:

  // create ozBSP from a Quake 3 QBSP and optimise it
  void build(const char* name);

};

extern BSP bsp;

}
