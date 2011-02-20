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

namespace oz
{
namespace client
{

  class Compiler;

  struct Vertex
  {
    float pos[3];
    float normal[3];
    float texCoord[2];

    bool operator == ( const Vertex& v ) const;

    void set( float px = 0.0f, float py = 0.0f, float pz = 0.0f,
              float nx = 0.0f, float ny = 0.0f, float nz = 0.0f,
              float ts = 0.0f, float tt = 0.0f );

    void set( const Point3& p = Point3::ORIGIN,
              const Vec3& n = Vec3::ZERO,
              float ts = 0.0f, float tt = 0.0f );
  };

  class Mesh
  {
    friend class Compiler;

    private:

      static const int TEXTURE0_BIT = 0x00000001;
      static const int TEXTURE1_BIT = 0x00000002;
      static const int BLEND_BIT    = 0x00000010;

      struct Part
      {
        Quat diffuse;
        Quat specular;
        uint texture[3];

        int  flags;

        int  firstElement;
        int  nElements;
      };

      uint arrayBuffer;
      uint elementBuffer;

      Vector<Part> parts;

    public:

      static void begin();
      static void end();

      void loadStatic( InputStream* stream );
      void loadStreaming( InputStream* stream );
      void unload();

      void draw() const;

  };

  inline bool Vertex::operator == ( const Vertex& v ) const
  {
    return pos[0] == v.pos[0] && pos[1] == v.pos[1] && pos[2] == v.pos[2] &&
        normal[0] == v.normal[0] && normal[1] == v.normal[1] && normal[2] == v.normal[2] &&
        texCoord[0] == v.texCoord[0] && texCoord[1] == v.texCoord[1];
  }

  inline void Vertex::set( float px, float py, float pz,
                           float nx, float ny, float nz,
                           float ts, float tt )
  {
    pos[0] = px;
    pos[1] = py;
    pos[2] = pz;

    normal[0] = nx;
    normal[1] = ny;
    normal[2] = nz;

    texCoord[0] = ts;
    texCoord[1] = tt;
  }

  inline void Vertex::set( const Point3& p, const Vec3& n, float ts, float tt )
  {
    pos[0] = p.x;
    pos[1] = p.y;
    pos[2] = p.z;

    normal[0] = n.x;
    normal[1] = n.y;
    normal[2] = n.z;

    texCoord[0] = ts;
    texCoord[1] = tt;
  }

}
}
