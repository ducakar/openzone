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
 * @file client/Mesh.hpp
 */

#pragma once

#include "client/common.hpp"

namespace oz
{
namespace client
{

struct Vertex
{
  float pos[3];
  float texCoord[2];
  float normal[3];
#ifdef OZ_BUMPMAP
  float tangent[3];
  float binormal[3];
#endif

  Vertex() = default;

#ifndef OZ_BUMPMAP
  explicit Vertex( const Point3& pos,
                   const TexCoord& texCoord = TexCoord( 0.0f, 0.0f ),
                   const Vec3& normal = Vec3::ZERO );
#else
  explicit Vertex( const Point3& pos,
                   const TexCoord& texCoord = TexCoord( 0.0f, 0.0f ),
                   const Vec3& normal = Vec3::ZERO,
                   const Vec3& tangent = Vec3::ZERO,
                   const Vec3& binormal = Vec3::ZERO );
#endif

  bool operator == ( const Vertex& v ) const;

  void read( InputStream* stream );
  void write( BufferStream* stream ) const;

  static void setFormat();
};

class Mesh
{
  public:

    static const int COMPONENT_MASK  = 0x00ff;
    static const int SOLID_BIT       = 0x0100;
    static const int ALPHA_BIT       = 0x0200;
    static const int EMBEDED_TEX_BIT = 0x0400;

  private:

    struct Part
    {
      int   flags;
      uint  mode;

      uint  texture;
      uint  masks;
      float alpha;
      float specular;

      int   nIndices;
      int   firstIndex;
    };

    uint         vao;
    uint         vbo;
    uint         ibo;

    int          flags;
    int          nParts;
    Part*        parts;
    DArray<int>  texIds;

  public:

    Mesh();
    ~Mesh();

    void load( InputStream* stream, uint usage );
    void unload();

    void upload( const Vertex* vertices, int nVertices, uint usage ) const;
    Vertex* map( uint access ) const;
    void unmap() const;

    void bind() const;
    void drawComponent( int id, int mask ) const;
    void draw( int mask ) const;

};

}
}
