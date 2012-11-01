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
 * @file client/Shape.hh
 */

#pragma once

#include <client/common.hh>

namespace oz
{
namespace client
{

class Shape
{
  private:

    struct Vertex
    {
      float pos[3];
      float texCoord[2];
      float normal[3];
    };

    static const Vertex VERTICES[];
    static const ushort INDICES[];

    uint vbo;
    uint ibo;

  public:

    explicit Shape();

    void bind() const;
    void unbind() const;

    static void colour( const Vec4& c );
    static void colour( float r, float g, float b, float a = 1.0f );

    static void fill( float x, float y, float width, float height );
    static void fill( int x, int y, int width, int height );
    static void fillInv( float x, float y, float width, float height );
    static void fillInv( int x, int y, int width, int height );
    static void rect( float x, float y, float width, float height );
    static void rect( int x, int y, int width, int height );
    static void tag( float minX, float minY, float maxX, float maxY );

    static void quad( float dimX, float dimY );
    static void box( const AABB& bb );
    static void wireBox( const AABB& bb );
    static void object( const Point& pos, const Mat33& rot, const void* shape );

    void init();
    void free();

};

extern Shape shape;

}
}
