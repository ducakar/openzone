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

  static const int CAP_UNIQUE = 0x00000001;
  static const int CAP_CW     = 0x00000002;
  static const int CAP_BLEND  = 0x00000004;

  class Compiler
  {
    private:

      static const int MESH_BIT     = 0x00000001;
      static const int SURFACE_BIT  = 0x00000002;

      struct Part
      {
        Quat   diffuse;
        Quat   specular;
        String texture[3];

        int    mode;

        Vector<int> indices;

        bool operator == ( const Part& part ) const;
      };

      Vector<Vertex> vertices;
      Vector<Part>   solidParts;
      Vector<Part>   alphaParts;

      Vertex         vert;
      Part           part;

      int            caps;
      int            flags;
      int            mode;
      int            vertNum;

    public:

      void enable( int cap );
      void disable( int cap );

      void beginMesh();
      void endMesh();

      void material( int face, int target, const float* params );
      void texture( int unit, const char* texture );

      void begin( int mode );
      void end();

      void texCoord( float u, float v );
      void texCoord( const float* v );

      void normal( float nx, float ny, float nz );
      void normal( const float* v );

      void vertex( float x, float y, float z );
      void vertex( const float* v );

      void index( int i );

      void getMeshData( MeshData* mesh ) const;

      void init();
      void free();

  };

  extern Compiler compiler;

}
}
