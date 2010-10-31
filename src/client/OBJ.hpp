/*
 *  OBJ.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "./stable.hpp"

#include "matrix/common.hpp"

namespace oz
{
namespace client
{

  class OBJ
  {
    private:

      static const int LINE_BUFFER_SIZE = 1024;

      struct Face
      {
        struct Vertex
        {
          // vertex position index in positions array
          int position;
          // vertex normal in normals array
          int normal;
          // vertex texture coordinates in texCoords array
          int texCoord;
          // vertex index in vertex buffer (temporary)
          mutable int index;

          explicit Vertex();
          explicit Vertex( int pos_, int norm_, int texCoord_ );

          bool operator == ( const Vertex& v ) const;
          bool operator != ( const Vertex& v ) const;
          bool operator < ( const Vertex& v ) const;
        };

        int            material;
        Vector<Vertex> vertices;
      };

      struct Vertex
      {
        Vec3     position;
        Vec3     norm;
        TexCoord texCoord;
      };

      struct Range
      {
        int material ;
        int firstVertex;
        int nVertices;
      };

      struct Material
      {
        Vec3 ambient;
        Vec3 specular;
        Quat diffuse;
        uint texId;
      };

      static Vector<Vec3>     positions;
      static Vector<Vec3>     normals;
      static Vector<TexCoord> texCoords;
      static Vector<Face>     faces;
      static Vector<Material> materials;
      static HashString<int>  materialIndices;

      static DArray<Vertex>   vertices;
      static DArray<uint>     indices;

      String name;

      uint   arrayBuffer;
      uint   indexBuffer;

      static char* skipSpaces( char* pos );
      static char* readWord( char* pos );

      bool readVertexData( char* pos ) const;
      bool readFace( char* pos, Face* face ) const;
      bool loadMaterial( const String& path );

      void saveCached( const char* fileName );

    public:

      uint list;

      static void prebuild( const char* name );

      explicit OBJ( const char* name );
      explicit OBJ( const char* name, int );
      ~OBJ();

      void draw() const;
      void genList();

      void trim();

  };

}
}
