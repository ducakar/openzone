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

#include <client/Model.hh>

namespace oz
{
namespace builder
{

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

  public:

    void enable( Capability cap );
    void disable( Capability cap );

    void beginModel();
    void endModel();

    void shader( const char* shaderName );

    void anim( int nFrames, int nPositions );
    void animPositions( const float* positions );
    void animNormals( const float* normals );

    void beginMesh( PolyMode mode );
    int endMesh();

    void texture( const char* texture );
    void blend( bool doBlend );

    void texCoord( float u, float v );
    void texCoord( const float* v );

    void normal( float nx, float ny, float nz );
    void normal( const float* v );

    void vertex( float x, float y, float z );
    void vertex( const float* v );

    void animVertex( int i );

    void beginNode();
    void endNode();

    void transform( const Mat44& t );
    void bindMesh( int id );

    void writeModel( OutputStream* os, bool globalTextures = false );
    void buildModelTextures( const char* destDir );

    void init();
    void destroy();

};

extern Compiler compiler;

}
}
