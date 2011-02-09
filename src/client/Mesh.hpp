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

  class Mesh
  {
    friend class Compiler;

    private:

      static const int TEXTURE0_BIT = 0x00000001;
      static const int TEXTURE1_BIT = 0x00000002;
      static const int BLEND_BIT    = 0x00000010;

      struct Vertex
      {
        float pos[3];
        float normal[3];
        float texCoord0[2];
        float texCoord1[2];

        bool operator == ( const Vertex& v ) const
        {
          return pos[0] == v.pos[0] && pos[1] == v.pos[1] && pos[2] == v.pos[2] &&
              normal[0] == v.normal[0] && normal[1] == v.normal[1] && normal[2] == v.normal[2] &&
              texCoord0[0] == v.texCoord0[0] && texCoord0[1] == v.texCoord0[1] &&
              texCoord1[0] == v.texCoord1[0] && texCoord1[1] == v.texCoord1[1];
        }
      };

      struct Part
      {
        Quat diffuse;
        Quat specular;
        uint texture0;
        uint texture1;

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

}
}
