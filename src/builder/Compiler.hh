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
 * @file builder/Compiler.hh
 */

#pragma once

#include <builder/common.hh>

#include <client/Mesh.hh>

namespace oz
{
namespace builder
{

struct Vertex
{
  Point    pos;
  TexCoord texCoord;
  Vec3     normal;

  bool operator == ( const Vertex& v ) const
  {
    return pos == v.pos && texCoord == v.texCoord && normal == v.normal;
  }

  void write( OutputStream* ostream ) const;
};

class Compiler
{
  public:

    enum PolyMode
    {
      TRIANGLE_STRIP,
      TRIANGLE_FAN,
      TRIANGLES,
      QUADS,
      POLYGON
    };

    enum Capability
    {
      UNIQUE    = 0x01,
      BUMPMAP   = 0x02,
      BLEND     = 0x04,
      CLOCKWISE = 0x08
    };

  private:

    static const int MESH_BIT = 0x00000001;
    static const int PART_BIT = 0x00000002;

    struct Part
    {
      int          component;

      int          material;
      String       texture;

      int          firstIndex;
      int          nIndices;
      List<ushort> indices;

      bool operator == ( const Part& part ) const
      {
        return component == part.component && material == part.material &&
               texture.equals( part.texture );
      }
    };

    List<Part>    parts;
    List<Vertex>  vertices;
    DArray<Point> positions;
    DArray<Vec3>  normals;

    Bounds        bounds;

    Vertex        vert;
    Part          part;

    int           caps;
    int           flags;
    int           componentId;
    PolyMode      mode;
    String        shaderName;
    int           vertNum;

    int           nFrames;
    int           nFramePositions;

  public:

    void enable( Capability cap );
    void disable( Capability cap );

    void beginMesh();
    void endMesh();

    void anim( int nFrames, int nPositions );
    void component( int id );
    void blend( bool doBlend );
    void shader( const char* shaderName );
    void texture( const char* texture );

    void begin( PolyMode mode );
    void end();

    void texCoord( float u, float v );
    void texCoord( const float* v );

    void normal( float nx, float ny, float nz );
    void normal( const float* v );

    void vertex( float x, float y, float z );
    void vertex( const float* v );

    void animVertex( int i );

    void animPositions( const float* positions );
    void animNormals( const float* normals );

    void writeMesh( OutputStream* os, bool globalTextures = false );
    void buildMeshTextures( const char* destDir );

    void init();
    void destroy();

};

extern Compiler compiler;

}
}
