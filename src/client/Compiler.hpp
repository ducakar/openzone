/*
 *  Compiler.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#ifdef OZ_TOOLS

#include "client/Mesh.hpp"

#include "client/OpenGL.hpp"

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

      static const int MESH_BIT = 0x00000001;
      static const int PART_BIT = 0x00000002;

      struct Part
      {
        int    component;
        uint   mode;

        String texture;
        float  alpha;
        float  specular;

        Vector<int> indices;

        bool operator == ( const Part& part ) const
        {
          return component == part.component && mode == part.mode && specular == part.specular &&
              alpha == part.alpha && texture.equals( part.texture );
        }
      };

      Vector<Vertex> vertices;
      Vector<Part>   parts;

      Vertex         vert;
      Part           part;

      int            caps;
      int            flags;
      int            componentId;
      uint           mode;
      int            vertNum;

    public:

      void enable( int cap );
      void disable( int cap );

      void beginMesh();
      void endMesh();

      void component( int id );
      void material( int target, float param );
      void texture( const char* texture );

      void begin( uint mode );
      void end();

      void texCoord( float u, float v );
      void texCoord( const float* v );

      void normal( float nx, float ny, float nz );
      void normal( const float* v );

      void vertex( float x, float y, float z );
      void vertex( const float* v );

      void animVertex( int i );

      void getMeshData( MeshData* mesh ) const;

      void init();
      void free();

  };

  extern Compiler compiler;

}
}

#endif
