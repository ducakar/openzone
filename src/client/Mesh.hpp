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
    float texCoord[6];

    bool operator == ( const Vertex& v ) const;

    void set( float px = 0.0f, float py = 0.0f, float pz = 0.0f,
              float nx = 0.0f, float ny = 0.0f, float nz = 0.0f,
              float t0s = 0.0f, float t0t = 0.0f,
              float t1s = 0.0f, float t1t = 0.0f,
              float t2s = 0.0f, float t2t = 0.0f );

    void set( const Point3& p = Point3::ORIGIN,
              const Vec3& n = Vec3::ZERO,
              const TexCoord& t0 = TexCoord( 0.0f, 0.0f ),
              const TexCoord& t1 = TexCoord( 0.0f, 0.0f ),
              const TexCoord& t2 = TexCoord( 0.0f, 0.0f ) );

    void read( InputStream* stream );
    void write( OutputStream* stream ) const;
  };

  class Mesh
  {
    friend class MeshData;
    friend class Compiler;

    static const int STREAMING_BIT = 0x00000001;

    private:

      struct Part
      {
        Quat diffuse;
        Quat specular;
        uint texture[3];

        int  mode;
        int  flags;

        int  firstIndex;
        int  nIndices;
      };

      uint         arrayId;
      int          flags;
      Vector<Part> solidParts;
      Vector<Part> alphaParts;

    public:



      static void begin();
      static void end();

      void load( InputStream* stream, int flags = 0 );
      void unload();

      Vertex* map( int access ) const;
      void unmap() const;

      void drawSolid() const;
      void drawAlpha() const;

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
        int    flags;

        int    firstIndex;
        int    nIndices;
      };

      Vector<Part>   solidParts;
      Vector<Part>   alphaParts;

      DArray<ushort> indices;
      DArray<Vertex> vertices;

      int getSize() const;
      void write( OutputStream* stream ) const;

  };

}
}
