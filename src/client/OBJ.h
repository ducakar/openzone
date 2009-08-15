/*
 *  OBJ.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
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

        uint *vertIndices;
        uint *normIndices;
        uint *texCoordIndices;
      };

      struct TexCoord
      {
        float u;
        float v;

        TexCoord() {}
        TexCoord( float u_, float v_ ) : u( u_ ), v( v_ ) {}
      };

      String           name;

      DArray<Vec3>     vertices;
      DArray<Vec3>     normals;
      DArray<TexCoord> texCoords;
      DArray<Face>     faces;

      uint             textureId;

      static char *skipSpaces( char *pos );
      static char *readWord( char *pos );

      bool readVertexData( char *pos,
                           Vector<Vec3> *tempVerts,
                           Vector<Vec3> *tempNormals,
                           Vector<TexCoord> *tempTexCoords ) const;
      bool readFace( char *pos, Face *face ) const;
      bool loadMaterial( const String &path );

    public:

      uint list;

      OBJ( const char *name );
      ~OBJ();

      void scale( float scale );
      void translate( const Vec3 &t );

      void draw() const;
      void genList();

      void trim();

  };

}
}
