/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file builder/MD2.hh
 */

#pragma once

#include <builder/common.hh>
#include <client/Model.hh>

namespace oz
{
namespace builder
{

class MD2
{
private:

  struct MD2Header
  {
    char id[4];
    int  version;

    int  skinWidth;
    int  skinHeight;
    int  frameSize;

    int  nSkins;
    int  nFramePositions;
    int  nTexCoords;
    int  nTriangles;
    int  nGlCmds;
    int  nFrames;

    int  offSkins;
    int  offTexCoords;
    int  offTriangles;
    int  offFrames;
    int  offGLCmds;
    int  offEnd;
  };

  struct MD2Vertex
  {
    ubyte p[3];
    ubyte normal;
  };

  struct MD2TexCoord
  {
    short s;
    short t;
  };

  struct MD2Frame
  {
    float     scale[3];
    float     translate[3];
    char      name[16];
    MD2Vertex verts[1];
  };

  struct MD2Triangle
  {
    short vertices[3];
    short texCoords[3];
  };

  static const Vec3 NORMALS[];

public:

  void build(const File& path);

};

extern MD2 md2;

}
}
