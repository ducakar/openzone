/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file build/OBJ.hh
 */

#pragma once

#include "build/common.hh"
#include "client/Mesh.hh"

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
      String masks;
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

    void readVertexData( char* pos );
    void readFace( char* pos, int part );
    void loadMaterials( const String& path );

    void load();
    void save();

    explicit OBJ( const char* path );

  public:

    static void build( const char* path );

};

}
}
