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
        String texture0;
        String texture1;

        int    flags;

        int    firstElement;
        int    nElements;
      };

      Vector<Mesh::Vertex> vertices;
      Vector<uint>         elements;
      Vector<Part>         parts;

      Part                 part;
      Mesh::Vertex         vertex;

      int                  flags;
      int                  mode;
      int                  vertNum;

    public:

      void glNewMesh();
      void glEndMesh();

      void glBeginMaterial();
      void glEndMaterial();

      void glEnable( int cap );
      void glMaterial( int face, int target, const float* params );
      void glBindTexture( int unit, int target, const char* texture );

      void glBegin( int mode );
      void glEnd();

      void glTexCoord( int target, float s, float t );
      void glTexCoord( int target, const float* v );

      void glNormal( float nx, float ny, float nz );
      void glNormal( const float* v );

      void glVertex( float x, float y, float z );
      void glVertex( const float* v );

      void writeMesh( OutputStream* stream ) const;
      void getMesh( Mesh* mesh ) const;

      void init();
      void free();

  };

  extern Compiler compiler;

}
}
