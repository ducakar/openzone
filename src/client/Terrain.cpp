/*
 *  Terrain.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Terrain.h"

#include "matrix/World.h"

#include "Context.h"
#include "Frustum.h"

#ifdef __WIN32__
static PFNGLMULTITEXCOORD2IPROC glMultiTexCoord2i = null;
static PFNGLMULTITEXCOORD2FPROC glMultiTexCoord2f = null;
static PFNGLACTIVETEXTUREPROC   glActiveTexture = null;
#endif

namespace oz
{
namespace client
{

  const float Terrain::DETAILTEX_SCALE = 4.0f;

  void Terrain::init()
  {
    detailTexId = context.loadTexture( "terra/detail.jpg" );
    mapTexId = context.loadTexture( "terra/map.png" );

#ifdef __WIN32__
    glMultiTexCoord2i = (PFNGLMULTITEXCOORD2IPROC) SDL_GL_GetProcAddress( "glMultiTexCoord2i" );
    glMultiTexCoord2f = (PFNGLMULTITEXCOORD2FPROC) SDL_GL_GetProcAddress( "glMultiTexCoord2f" );
    glActiveTexture   = (PFNGLACTIVETEXTUREPROC)   SDL_GL_GetProcAddress( "glActiveTexture" );
#endif

    int lineVerts = oz::Terrain::MAX + 1;
    int nVertices = lineVerts * lineVerts;

    normals   = new Vec3[nVertices];
    texCoords = new float[nVertices][2];
    vertices  = new Vec3[nVertices];

    for( int x = 0, i = 0; x <= oz::Terrain::MAX; x++ ) {
      for( int y = 0; y <= oz::Terrain::MAX; y++, i++ ) {
        vertices[i] = Vec3( x * oz::TerraQuad::DIM - oz::Terrain::DIM,
                            y * oz::TerraQuad::DIM - oz::Terrain::DIM,
                            world.terrain.heightMap[x][y] );
      }
    }


  }

  void Terrain::free()
  {
    if( normals != null ) {
      delete[] normals;
      normals = null;
    }
    if( texCoords != null ) {
      delete[] texCoords;
      texCoords = null;
    }
    if( vertices != null ) {
      delete[] vertices;
      vertices = null;
    }
  }

  void Terrain::draw() const
  {}

}
}
