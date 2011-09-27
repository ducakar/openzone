/*
 *  BSP.hpp
 *
 *  BSP level rendering class
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Struct.hpp"
#include "matrix/BSP.hpp"

#include "client/Mesh.hpp"

namespace oz
{
namespace client
{

  class BSP
  {
    private:

      static const int LIGHTMAP_DIM  = 128;
      static const int LIGHTMAP_BPP  = 3;
      static const int LIGHTMAP_SIZE = LIGHTMAP_DIM * LIGHTMAP_DIM * LIGHTMAP_BPP;

#ifndef OZ_TOOLS

      struct Model
      {
        Mesh mesh;
        int  openSample;
        int  closeSample;
        int  frictSample;
      };

      int           id;
      DArray<Model> models;

      int           flags;

      void playSound( const Struct::Entity* entity, int sample ) const;
      void playContSound( const Struct::Entity* entity, int sample ) const;

    public:

      bool isLoaded;

      explicit BSP( int id );
      ~BSP();

      void load();
      void draw( const Struct* str, int mask ) const;
      void play( const Struct* str ) const;

#else

      static const int QBSP_SLICK_FLAG_BIT    = 0x00000002;
      static const int QBSP_LADDER_FLAG_BIT   = 0x00000008;
      static const int QBSP_GLASS_FLAG_BIT    = 0x00000020;
      static const int QBSP_NONSOLID_FLAG_BIT = 0x00004000;
      static const int QBSP_WATER_TYPE_BIT    = 0x00000020;

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

      struct QBSPModel
      {
        float bb[2][3];

        int firstFace;
        int nFaces;

        int firstBrush;
        int nBrushes;
      };

      struct QBSPVertex
      {
        float p[3];
        float texCoord[2];
        float lightmapCoord[2];
        float normal[3];
        char  colour[4];
      };

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

      struct ModelSamples
      {
        String openSample;
        String closeSample;
        String frictSample;
      };

      static int nTextures;
      static int nModels;
      static int nVertices;
      static int nIndices;
      static int nFaces;

      static DArray<QBSPTexture>  textures;
      static DArray<QBSPModel>    models;
      static DArray<QBSPVertex>   vertices;
      static DArray<int>          indices;
      static DArray<QBSPFace>     faces;
      static DArray<ModelSamples> modelSamples;

      // prebuild
      static void loadQBSP( const char* path );
      static void freeQBSP();
      static void optimise();
      static void save( const char* path );

    public:

      // create ozcBSP from a Quake 3 QBSP (matrix BSP must be loaded)
      static void prebuild( const char* name );

#endif

  };

}
}
