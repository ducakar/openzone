/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file build/OBJ.hpp
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

    explicit OBJ( const char* path );

  public:

    static void build( const char* path );

};

}
}
