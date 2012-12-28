/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file builder/OBJ.hh
 */

#pragma once

#include <builder/common.hh>
#include <client/Mesh.hh>

namespace oz
{
namespace builder
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

      explicit FaceVertex() = default;

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
      List<FaceVertex> vertices;
    };

    struct Part
    {
      List<Face> faces;

      String     texture;
      String     masks;
      float      alpha;
    };

    String               path;
    String               shader;
    List<Point>          positions;
    List<Vec3>           normals;
    List<TexCoord>       texCoords;
    List<Part>           parts;
    HashMap<String, int> materialIndices;

    static char* skipSpaces( char* pos );
    static char* readWord( char* pos );

    void readVertexData( const char* pos );
    void readFace( char* pos, int part );
    void loadMaterials( const char* filePath );

    void load();
    void save();

  public:

    void build( const char* path );

};

extern OBJ obj;

}
}
