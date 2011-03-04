/*
 *  Mesh.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/common.hpp"

namespace oz
{
namespace client
{

  class MeshData;
  class Compiler;

  struct Vertex
  {
    float pos[3];
    float texCoord[2];
    float normal[3];
//     float tangent[3];
//     float binormal[3];

    bool operator == ( const Vertex& v ) const;

    void set( float x = 0.0f, float y = 0.0f, float z = 0.0f,
              float u = 0.0f, float v = 0.0f,
              float nx = 0.0f, float ny = 0.0f, float nz = 0.0f,
              float tx = 0.0f, float ty = 0.0f, float tz = 0.0f,
              float bx = 0.0f, float by = 0.0f, float bz = 0.0f );

    void set( const Point3& p = Point3::ORIGIN,
              const TexCoord& c = TexCoord( 0.0f, 0.0f ),
              const Vec3& n = Vec3::ZERO,
              const Vec3& t = Vec3::ZERO,
              const Vec3& b = Vec3::ZERO );

    void read( InputStream* stream );
    void write( OutputStream* stream ) const;
  };

  class Mesh
  {
    friend class MeshData;
    friend class Compiler;

    public:

      static const int FIRST_ALPHA_PART_MASK = 0x00ff;
      static const int SOLID_BIT             = 0x0100;
      static const int ALPHA_BIT             = 0x0200;
      static const int EMBEDED_TEX_BIT       = 0x0400;

    private:

      struct Part
      {
        Quat   diffuse;
        Quat   specular;
        uint   texture[3];

        int    mode;

        int    nIndices;
        int    firstIndex;
      };

      uint         vao;
      uint         ibo;
      uint         vbo;

      int          flags;
      DArray<int>  texIds;
      DArray<Part> parts;

    public:

      explicit Mesh();
      ~Mesh();

      void load( InputStream* stream, int usage );
      void unload();

      void upload( const Vertex* vertices, int nVertices, int flags = 0 ) const;
      Vertex* map( int access ) const;
      void unmap() const;

      void draw( int mask ) const;

  };

  class MeshData
  {
    friend class Compiler;

    public:

      struct Part
      {
        Quat   diffuse;
        Quat   specular;
        String texture[3];

        int    mode;

        int    nIndices;
        int    firstIndex;
      };

      Vector<Part>   solidParts;
      Vector<Part>   alphaParts;

      DArray<ushort> indices;
      DArray<Vertex> vertices;

      void write( OutputStream* stream, bool embedTextures = true ) const;

  };

}
}
