/*
 *  OBJ.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

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
          int iPos;
          // vertex normal in normals array
          int iNorm;
          // vertex texture coordinates in texCoords array
          int iTexCoord;
          // vertex index in vertex buffer
          int iVertex;

          explicit Vertex()
          {}

          explicit Vertex( int iPos_, int iNorm_, int iTexCoord_ ) :
              iPos( iPos_ ), iNorm( iNorm_ ), iTexCoord( iTexCoord_ )
          {}

          bool operator == ( const Vertex& v ) const
          {
            return iPos == v.iPos && iNorm == v.iNorm && iTexCoord == v.iTexCoord;
          }

          bool operator != ( const Vertex& v ) const
          {
            return iPos != v.iPos || iNorm != v.iNorm || iTexCoord != v.iTexCoord;
          }

          bool operator < ( const Vertex& v ) const
          {
            return iPos < v.iPos ||
                ( iPos == v.iPos && ( iNorm < v.iNorm ||
                    ( iNorm == v.iNorm && iTexCoord < v.iTexCoord ) ) );
          }
        };

        int            iMaterial;
        Vector<Vertex> vertices;
      };

      struct Vertex
      {
        Vec3     pos;
        Vec3     norm;
        TexCoord texCoord;
      };

      struct Range
      {
        int iMaterial;
        int iFirstIndex;
        int iLastIndex;
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

      String name;

      static char* skipSpaces( char* pos );
      static char* readWord( char* pos );

      bool readVertexData( char* pos ) const;
      bool readFace( char* pos, Face* face ) const;
      bool loadMaterial( const String& path );

    public:

      uint list;

      explicit OBJ( const char* name );
      ~OBJ();

      void draw() const;
      void genList();
      void saveCached( const char* fileName );

      void trim();

  };

}
}
