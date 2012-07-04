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
 * @file client/DMesh.hh
 */

#pragma once

#include "client/Mesh.hh"

namespace oz
{
namespace client
{

class DMesh : public Mesh
{
  private:

    static Vertex* vertexAnimBuffer;
    static int     vertexAnimBufferLength;

    uint    positionsTexId;
    uint    normalsTexId;

    int     nFrames;
    int     nFramePositions;
    int     nFrameVertices;

    Vertex* vertices;
    Point*  positions;
    Vec3*   normals;

  public:

    DMesh();

    void drawFrame( int mask, int frame ) const;
    void drawAnim( int mask, int firstFrame, int secondFrame, float interpolation ) const;

    void load( InputStream* istream, const char* path );
    void unload();

    static void free();

};

}
}
