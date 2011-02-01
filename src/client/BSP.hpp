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

  class BSP
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

      struct Vertex
      {
        Point3   p;
        Vec3     normal;
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

      static Point3  camPos;
      static int     waterFlags;

      const oz::BSP* bsp;
      String         name;

      int            nTextures;
      int            nEntityModels;
      int            nVertices;
      int            nIndices;
      int            nFaces;
      int            nLightmaps;

      int*           textures;
      EntityModel*   entityModels;
      Vertex*        vertices;
      int*           indices;
      Face*          faces;
      Lightmap*      lightmaps;

      uint           baseList;
      uint*          texIds;
      uint*          lightmapIds;

      Bitset         drawnFaces;
      Bitset         visibleLeafs;
      Bitset         hiddenFaces;

      bool isInWater() const;

      void drawFace( const Face* face ) const;
      void drawFaceWater( const Face* face ) const;

      bool loadOZCBSP( const char* fileName );
      void freeOZCBSP();

      // prebuild
      bool loadQBSP( const char* fileName );
      void freeQBSP( const char* name );
      void optimise();
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

      int  fullDraw( const Struct* str );
      void fullDrawWater( const Struct* str );

      static void beginRender();
      static void endRender();

  };

}
}
