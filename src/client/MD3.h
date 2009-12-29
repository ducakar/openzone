/*
 *  MD3.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "matrix/matrix.h"
#include "matrix/Timer.h"

namespace oz
{
namespace client
{

  struct MD3Tag;

  struct MD3
  {
    private:

      struct Triangle
      {
        int indices[3];
      };

      struct TexCoord
      {
        float u;
        float v;
      };

      struct Vertex
      {
        Vec3 p;
        Vec3 normal;
      };

      struct Mesh
      {
        int  nVertices;  // number of vertices per frame, not all vertices
        uint texId;

        DArray<Triangle> triangles;
        DArray<TexCoord> texCoords;
        DArray<Vertex>   vertices;
      };

      struct Offset
      {
        Vec3 p;
        Quat rot;
      };

      struct Part
      {
        friend class MD3;

        private:

          DArray<Mesh> meshes;
          int          nFrames;

        public:

          Part( MD3* parent, const String& dir, const char* fileName, MD3Tag** tags );
          ~Part();

          void scale( float scale );
          void translate( const Vec3& t );
          void drawFrame( int frame ) const;

      };

      Vector<uint>   textures;

      Part*          head;
      Part*          upper;
      Part*          lower;

      DArray<Offset> headOffsets;
      DArray<Offset> lowerOffsets;
      DArray<Offset> weaponOffsets;

      void scale( float scale );
      void translate( const Vec3& t );

    public:

      uint list;

      explicit MD3( const char* name );
      ~MD3();

      void drawFrame( int frame ) const;
      void genList();

      // call on static models, to release resources after list has been generated
      void trim();

  };

}
}
