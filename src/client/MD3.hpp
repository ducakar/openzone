/*
 *  MD3.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/common.hpp"
#include "matrix/Timer.hpp"

#include "client/common.hpp"

namespace oz
{
namespace client
{

  struct MD3Tag;

  class MD3
  {
    private:

#ifndef OZ_TOOLS

//       struct Triangle
//       {
//         int indices[3];
//       };
//
//       struct Vertex
//       {
//         Point3 p;
//         Vec3   normal;
//       };
//
//       struct Mesh
//       {
//         int  nVertices;  // number of vertices per frame, not all vertices
//         uint texId;
//
//         DArray<Triangle> triangles;
//         DArray<TexCoord> texCoords;
//         DArray<Vertex>   vertices;
//       };
//
//       struct Offset
//       {
//         Vec3 p;
//         Quat rot;
//       };
//
//       class Part
//       {
//         friend class MD3;
//
//         private:
//
//           DArray<Mesh> meshes;
//           int          nFrames;
//
//         public:
//
//           explicit Part( MD3* parent, const String& dir, const char* fileName, MD3Tag** tags );
//           ~Part();
//
//           void scale( float scale );
//           void translate( const Vec3& t );
//           void drawFrame( int frame ) const;
//
//       };
//
//       int            id;
//
//       Vector<uint>   textures;
//
//       Part*          head;
//       Part*          upper;
//       Part*          lower;
//
//       DArray<Offset> headOffsets;
//       DArray<Offset> lowerOffsets;
//       DArray<Offset> weaponOffsets;
//
//       void scale( float scale );
//       void translate( const Vec3& t );
//
//     public:
//
//       uint list;
//       bool isLoaded;
//
//       explicit MD3( int id );
//       ~MD3();
//
//       void load();
//
//       void drawFrame( int frame ) const;
//
//       void genList();
//       void deleteList() const;

    private:

      int id;

    public:

      Mat44   weaponTransf;
      bool    isLoaded;

      explicit MD3( int id );
      ~MD3();

      void load();

//       void advance( AnimState* anim, float dt ) const;

      void drawFrame( int frame ) const;
//       void draw( const AnimState* anim ) const;

#else // OZ_TOOLS

      struct MD3Header
      {
        char id[4];
        int  version;
        char fileName[64];
        int  flags;

        int  nFrames;
        int  nTags;
        int  nSurfaces;
        int  nSkins;

        int  offFrames;
        int  offTags;
        int  offSurfaces;
        int  offEnd;
      };

      struct MD3Frame
      {
        float mins[3];
        float maxs[3];
        float pos[3];
        float radius;
        char  name[16];
      };

      struct MD3Tag
      {
        char  name[64];
        float pos[3];
        float rot[9];
      };

      struct MD3Surface
      {
        int  id;
        char name[64];
        int  flags;

        int  nFrames;
        int  nShaders;
        int  nVertices;
        int  nTriangles;

        int  offTriangles;
        int  offShaders;
        int  offTexCoords;
        int  offVertices;
        int  offEnd;
      };

      struct MD3Triangle
      {
        int vertices[3];
      };

      struct MD3Shader
      {
        char name[64];
        int  index;
      };

      struct MD3TexCoord
      {
        float s;
        float t;
      };

      struct MD3Vertex
      {
        short pos[3];
        ubyte normal[2];
      };

      static String sPath;
      static Config config;

      static bool   forceStatic;
      static float  scale;
      static Mat44  meshTransf;
      static Vec3   jumpTransl;
      static Mat44  weaponTransf;

      static void buildMesh( const char* name, int frame, DArray<MD3Tag>* tags );

    public:

      static void prebuild( const char* path );

#endif // OZ_TOOLS

  };

}
}
