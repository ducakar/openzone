/*
 *  OBJ.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

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
        int nVerts;

        uint* vertIndices;
        uint* normIndices;
        uint* texCoordIndices;

        Face() : nVerts( 0 ), vertIndices( null ), normIndices( null ), texCoordIndices( null ) {}
      };

      struct TexCoord
      {
        float u;
        float v;

        TexCoord() {}
        TexCoord( float u_, float v_ ) : u( u_ ), v( v_ ) {}
      };

      struct Material
      {
        Vec4 ambient;
        Vec4 specular;
        Quat diffuse;
        uint texId;
      };

      String           name;

      DArray<Vec4>     vertices;
      DArray<Vec4>     normals;
      DArray<TexCoord> texCoords;
      DArray<Face>     faces;
      DArray<Material> materials;

      static char* skipSpaces( char* pos );
      static char* readWord( char* pos );

      bool readVertexData( char* pos,
                           Vector<Vec4>* tempVerts,
                           Vector<Vec4>* tempNormals,
                           Vector<TexCoord>* tempTexCoords ) const;
      bool readFace( char* pos, Face* face ) const;
      bool loadMaterial( const String& path, HashString<int, 32>* materialIndices );

    public:

      uint list;

      explicit OBJ( const char* name );
      ~OBJ();

      void scale( float scale );
      void translate( const Vec4& t );

      void draw() const;
      void genList();

      void trim();

  };

}
}
