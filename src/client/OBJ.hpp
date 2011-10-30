/*
 *  OBJ.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "client/common.hpp"

#ifdef OZ_TOOLS

#include "client/Mesh.hpp"

namespace oz
{
namespace client
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

    static String           shaderName;
    static Vector<Point3>   positions;
    static Vector<Vec3>     normals;
    static Vector<TexCoord> texCoords;
    static Vector<Part>     parts;
    static HashString<int>  materialIndices;

    String name;

    static char* skipSpaces( char* pos );
    static char* readWord( char* pos );

    static bool readVertexData( char* pos );
    static bool readFace( char* pos, int part );
    static bool loadMaterials( const String& path );

    static void loadOBJ( const char* name );
    static void freeOBJ();
    static void save( const char* fileName );

  public:

    static void prebuild( const char* name );

};

}
}

#endif
