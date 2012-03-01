/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file build/Compiler.hh
 */

#pragma once

#include "build/Mesh.hh"

namespace oz
{
namespace build
{

static const int CAP_UNIQUE = 0x00000001;
static const int CAP_CW     = 0x00000002;
static const int CAP_BLEND  = 0x00000004;

class Compiler
{
  private:

    static const int MESH_BIT = 0x00000001;
    static const int PART_BIT = 0x00000002;

    struct Part
    {
      int    component;
      uint   mode;

      String texture;
      float  alpha;
      float  specular;

      Vector<int> indices;

      bool operator == ( const Part& part ) const
      {
        return component == part.component && mode == part.mode &&
               specular == part.specular && alpha == part.alpha && texture.equals( part.texture );
      }
    };

    Vector<Vertex> vertices;
    Vector<Part>   parts;

    Vertex         vert;
    Part           part;

    int            caps;
    int            flags;
    int            componentId;
    uint           mode;
    int            vertNum;

  public:

    void enable( int cap );
    void disable( int cap );

    void beginMesh();
    void endMesh();

    void component( int id );
    void material( int target, float param );
    void texture( const char* texture );

    void begin( uint mode );
    void end();

    void texCoord( float u, float v );
    void texCoord( const float* v );

    void detailCoord( float u, float v );
    void detailCoord( const float* v );

    void normal( float nx, float ny, float nz );
    void normal( const float* v );

    void vertex( float x, float y, float z );
    void vertex( const float* v );

    void animVertex( int i );

    void getMeshData( MeshData* mesh ) const;

    void init();
    void free();

};

extern Compiler compiler;

}
}
