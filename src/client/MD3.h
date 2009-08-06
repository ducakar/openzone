/*
 *  MD3.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/bv.h"
#include "matrix/Timer.h"

namespace oz
{
namespace client
{

  class MD3
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

      class Part
      {
        private:

          DArray<Part*> links;
          DArray<Mesh>  meshes;

        public:

          Part( const char *path );
          ~Part();

          void drawFrame( int frame ) const;

      };

      Part *head;
      Part *torso;
      Part *legs;

    public:

      MD3( const char *name );
      ~MD3();

      void drawFrame( int frame ) const;

      static uint genList( const char *path );

  };

}
}
