/*
 *  MD3.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/common.hpp"
#include "matrix/Timer.hpp"

namespace oz
{
namespace client
{

  struct MD3Tag;

  class MD3
  {
    private:

      struct Triangle
      {
        int indices[3];
      };

      struct Vertex
      {
        Point3 p;
        Vec3   normal;
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

      class Part
      {
        friend class MD3;

        private:

          DArray<Mesh> meshes;
          int          nFrames;

        public:

          explicit Part( MD3* parent, const String& dir, const char* fileName, MD3Tag** tags );
          ~Part();

          void scale( float scale );
          void translate( const Vec3& t );
          void drawFrame( int frame ) const;

      };

      String         name;

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
      bool isLoaded;

      explicit MD3( const char* name );
      ~MD3();

      void load();

      void drawFrame( int frame ) const;

      void genList();
      void deleteList() const;

  };

}
}
