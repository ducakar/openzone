/*
 *  Mesh.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
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
  void write( OutputStream* stream ) const;

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
      float alpha;
      float specular;

      int   nIndices;
      int   firstIndex;
    };

    uint         vao;
    uint         vbo;
    uint         ibo;

    int          flags;
    DArray<int>  texIds;
    DArray<Part> parts;

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
