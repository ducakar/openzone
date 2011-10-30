/*
 *  OBJ.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "build/common.hpp"
#include "client/Mesh.hpp"

namespace oz
{
namespace build
{

struct Face;

class OBJ
{
  private:

    static const int LINE_BUFFER_SIZE = 1024;

    struct FaceVertex
    {
      // vertex position index in positions array
      int position;
      // vertex normal in normals array
      int normal;
      // vertex texture coordinates in texCoords array
      int texCoord;

      FaceVertex() = default;

      explicit FaceVertex( int pos_, int norm_, int texCoord_ ) :
          position( pos_ ), normal( norm_ ), texCoord( texCoord_ )
      {}

      // lexicographical order
      bool operator == ( const FaceVertex& v ) const
      {
        return position == v.position && normal == v.normal && texCoord == v.texCoord;
      }
    };

    struct Face
    {
      Vector<FaceVertex> vertices;
    };

    struct Part
    {
      Vector<Face> faces;

      String texture;
      float  alpha;
      float  specular;
    };

    String           path;
    String           shader;
    Vector<Point3>   positions;
    Vector<Vec3>     normals;
    Vector<TexCoord> texCoords;
    Vector<Part>     parts;
    HashString<int>  materialIndices;

    static char* skipSpaces( char* pos );
    static char* readWord( char* pos );

    bool readVertexData( char* pos );
    bool readFace( char* pos, int part );
    bool loadMaterials( const String& path );

    void load();
    void save();

    OBJ( const char* path );

  public:

    static void build( const char* path );

};

}
}
