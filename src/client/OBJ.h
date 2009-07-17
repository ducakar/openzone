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

      uint     textureId;

      int      nVertices;
      Vec3     *vertices;

      int      nNormals;
      Vec3     *normals;

      int      nTexCoords;
      TexCoord *texCoords;

      int      nFaces;
      Face     *faces;

      static char *skipSpaces( char *pos );
      static char *readWord( char *pos );

      bool readVertexData( char *pos,
                           Vector<Vec3> *tempVerts,
                           Vector<Vec3> *tempNormals,
                           Vector<TexCoord> *tempTexCoords ) const;
      bool readFace( char *pos, Face *face ) const;
      bool loadMaterial( const String &path );

    public:

      OBJ();
      ~OBJ();

      bool load( const char *name );
      void free();

      void draw() const;
      static uint genList( const char *name );

      void scale( float scale );
      void translate( const Vec3 &t );

  };

}
}
