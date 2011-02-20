/*
 *  Compiler.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/Mesh.hpp"

namespace oz
{
namespace client
{

  class Compiler
  {
    private:

      static const int MESH_BIT     = 0x00000001;
      static const int MATERIAL_BIT = 0x00000002;
      static const int SURFACE_BIT  = 0x00000004;

      struct Part
      {
        Quat   diffuse;
        Quat   specular;
        String texture[3];

        int    flags;

        int    firstElement;
        int    nElements;
      };

      Vector<Vertex> vertices;
      Vector<ushort> elements;
      Vector<Part>   parts;

      Part           part;
      Vertex         vert;

      int            flags;
      int            mode;
      int            vertNum;

    public:

      void beginMesh();
      void endMesh();

      void beginMaterial();
      void endMaterial();

      void enable( int cap );
      void disable( int cap );
      void material( int face, int target, const float* params );
      void setTexture( int unit, int target, const char* texture );

      void texCoord( float s, float t );
      void texCoord( const float* v );

      void normal( float nx, float ny, float nz );
      void normal( const float* v );

      void vertex( float x, float y, float z );
      void vertex( const float* v );

      void element( ushort index );

      void begin( int mode );
      void end();

      void writeMesh( OutputStream* stream ) const;
      void getMesh( Mesh* mesh ) const;

      void init();
      void free();

  };

  extern Compiler compiler;

}
}
