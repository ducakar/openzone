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
    float normal[3];
    float texCoord[2];

    bool operator == ( const Vertex& v ) const;

    void set( float x = 0.0f, float y = 0.0f, float z = 0.0f,
              float nx = 0.0f, float ny = 0.0f, float nz = 0.0f,
              float u = 0.0f, float v = 0.0f );

    void set( const Point3& p = Point3::ORIGIN,
              const Vec3& n = Vec3::ZERO,
              const TexCoord& t = TexCoord( 0.0f, 0.0f ) );

    void read( InputStream* stream );
    void write( OutputStream* stream ) const;
  };

  class Mesh
  {
    friend class MeshData;
    friend class Compiler;

    public:

      static const int SOLID_BIT       = 0x01;
      static const int ALPHA_BIT       = 0x02;
      static const int EMBEDED_TEX_BIT = 0x04;

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
      Vector<Part> solidParts;
      Vector<Part> alphaParts;

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
