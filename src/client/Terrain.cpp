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
#include "matrix/Terrain.h"

#include "Camera.h"
#include "Context.h"
#include "Frustum.h"

#ifdef __WIN32__
static PFNGLACTIVETEXTUREPROC glActiveTexture = null;
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
    glActiveTexture = (PFNGLACTIVETEXTUREPROC) SDL_GL_GetProcAddress( "glActiveTexture" );
#endif

    int nVertices = oz::Terrain::MAX * oz::Terrain::MAX;

    normals         = new Vec3[nVertices];
    detailTexCoords = new TexCoord[nVertices];
    mapTexCoords    = new TexCoord[nVertices];

    for( int x = 0; x < oz::Terrain::MAX; x++ ) {
      for( int y = 0; y < oz::Terrain::MAX; y++ ) {
        Vec3 &n = normals[x * oz::Terrain::MAX + y];

        n.setZero();
        if( x < oz::Terrain::QUADS && y < oz::Terrain::QUADS ) {
          n += world.terrain.quads[x][y].tri[0].normal;
          n += world.terrain.quads[x][y].tri[1].normal;
        }
        if( x > 0 && y < oz::Terrain::QUADS ) {
          n += world.terrain.quads[x - 1][y].tri[0].normal;
        }
        if( x > 0 && y > 0 ) {
          n += world.terrain.quads[x - 1][y - 1].tri[0].normal;
          n += world.terrain.quads[x - 1][y - 1].tri[1].normal;
        }
        if( x < oz::Terrain::QUADS && y > 0 ) {
          n += world.terrain.quads[x][y - 1].tri[1].normal;
        }
        n.norm();

        detailTexCoords[x * oz::Terrain::MAX + y].u = (float) ( x & 1 );
        detailTexCoords[x * oz::Terrain::MAX + y].v = (float) ( y & 1 );

        mapTexCoords[x * oz::Terrain::MAX + y].u = ((float) x) / oz::Terrain::MAX;
        mapTexCoords[x * oz::Terrain::MAX + y].v = ((float) y) / oz::Terrain::MAX;
      }
    }
  }

  void Terrain::free()
  {
    if( normals != null ) {
      delete[] normals;
      normals = null;
    }
    if( detailTexCoords != null ) {
      delete[] detailTexCoords;
      detailTexCoords = null;
    }
    if( mapTexCoords != null ) {
      delete[] mapTexCoords;
      mapTexCoords = null;
    }
  }

  void Terrain::draw() const
  {
    world.terrain.getInters( camera.p.x - radius, camera.p.y - radius,
                             camera.p.x + radius, camera.p.y + radius );

    float minX = world.terrain.minX;
    float minY = world.terrain.minY;
    float maxX = world.terrain.maxX + 1;
    float maxY = world.terrain.maxY + 1;

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, detailTexId );

    glActiveTexture( GL_TEXTURE1 );
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, mapTexId );

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    glVertexPointer( 3, GL_FLOAT, 0, world.terrain.vertices );
    glNormalPointer( GL_FLOAT, 0, normals );

    glClientActiveTexture( GL_TEXTURE0 );
    glTexCoordPointer( 2, GL_FLOAT, 0, detailTexCoords );

    glClientActiveTexture( GL_TEXTURE1 );
    glTexCoordPointer( 2, GL_FLOAT, 0, mapTexCoords );

    for( int y = minY; y < maxY; y++ ) {
      glBegin( GL_TRIANGLE_STRIP );
      for( int x = minX; x <= maxX; x++ ) {
        glArrayElement( x * oz::Terrain::MAX + y + 1 );
        glArrayElement( x * oz::Terrain::MAX + y     );
      }
      glEnd();
    }

    glActiveTexture( GL_TEXTURE1 );
    glDisable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE0 );

    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );
    glDisableClientState( GL_VERTEX_ARRAY );
  }

}
}
