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
#include "Water.h"

#ifdef __WIN32__
static PFNGLACTIVETEXTUREPROC   glActiveTexture   = null;
static PFNGLMULTITEXCOORD2IPROC glMultiTexCoord2i = null;
static PFNGLMULTITEXCOORD2FPROC glMultiTexCoord2f = null;
#endif

namespace oz
{
namespace client
{

  const float Terrain::DETAIL_SCALE = 4.0f;
  const float Terrain::WATER_SCALE  = 2.0f;

  void Terrain::init()
  {
    detailTexId = context.loadTexture( "terra/detail.jpg" );
    mapTexId    = context.loadTexture( "terra/map.png" );
    waterTexId  = context.loadTexture( "terra/water.jpg" );

#ifdef __WIN32__
    glActiveTexture   = (PFNGLACTIVETEXTUREPROC)   SDL_GL_GetProcAddress( "glActiveTexture" );
    glMultiTexCoord2i = (PFNGLMULTITEXCOORD2IPROC) SDL_GL_GetProcAddress( "glMultiTexCoord2i" );
    glMultiTexCoord2f = (PFNGLMULTITEXCOORD2FPROC) SDL_GL_GetProcAddress( "glMultiTexCoord2f" );
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
          n += world.terra.quads[x][y].tri[0].normal;
          n += world.terra.quads[x][y].tri[1].normal;
        }
        if( x > 0 && y < oz::Terrain::QUADS ) {
          n += world.terra.quads[x - 1][y].tri[0].normal;
        }
        if( x > 0 && y > 0 ) {
          n += world.terra.quads[x - 1][y - 1].tri[0].normal;
          n += world.terra.quads[x - 1][y - 1].tri[1].normal;
        }
        if( x < oz::Terrain::QUADS && y > 0 ) {
          n += world.terra.quads[x][y - 1].tri[1].normal;
        }
        n.norm();

        detailTexCoords[x * oz::Terrain::MAX + y].u = (float) ( x & 1 ) * DETAIL_SCALE;
        detailTexCoords[x * oz::Terrain::MAX + y].v = (float) ( y & 1 ) * DETAIL_SCALE;

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
    world.terra.getInters( camera.p.x - radius, camera.p.y - radius,
                           camera.p.x + radius, camera.p.y + radius );

    int minX = world.terra.minX;
    int minY = world.terra.minY;
    int maxX = world.terra.maxX + 1;
    int maxY = world.terra.maxY + 1;

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, detailTexId );

    glActiveTexture( GL_TEXTURE1 );
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, mapTexId );

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    glVertexPointer( 3, GL_FLOAT, 0, world.terra.vertices );
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

    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );
    glDisableClientState( GL_VERTEX_ARRAY );

    glActiveTexture( GL_TEXTURE1 );
    glDisable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE0 );
  }

  void Terrain::drawWater() const
  {
    world.terra.getInters( camera.p.x - radius, camera.p.y - radius,
                           camera.p.x + radius, camera.p.y + radius );

    int minX = world.terra.minX;
    int minY = world.terra.minY;
    int maxX = world.terra.maxX + 1;
    int maxY = world.terra.maxY + 1;

    glEnable( GL_BLEND );

    glBindTexture( GL_TEXTURE_2D, waterTexId );

    const Vec3 &v0 = world.terra.vertices[minX][minY];
    const Vec3 &v1 = world.terra.vertices[maxX][maxY];

    if( camera.p.z >= 0 ) {
      glNormal3f( 0.0f, 0.0f, 1.0f );

      glBegin( GL_QUADS );
        glColor4f( 1.0f, 1.0f, 1.0f, water.alpha1 );

        glTexCoord2f( minX * WATER_SCALE, minY * WATER_SCALE );
        glVertex3f( v0.x, v0.y, 0.0f );

        glTexCoord2f( maxX * WATER_SCALE, minY * WATER_SCALE );
        glVertex3f( v1.x, v0.y, 0.0f );

        glTexCoord2f( maxX * WATER_SCALE, maxY * WATER_SCALE );
        glVertex3f( v1.x, v1.y, 0.0f );

        glTexCoord2f( minX * WATER_SCALE, maxY * WATER_SCALE );
        glVertex3f( v0.x, v1.y, 0.0f );

        glColor4f( 1.0f, 1.0f, 1.0f, water.alpha2 );

        glTexCoord2f( minX * WATER_SCALE + Water::TEX_BIAS, minY * WATER_SCALE + Water::TEX_BIAS );
        glVertex3f( v0.x, v0.y, 0.0f );

        glTexCoord2f( maxX * WATER_SCALE + Water::TEX_BIAS, minY * WATER_SCALE + Water::TEX_BIAS );
        glVertex3f( v1.x, v0.y, 0.0f );

        glTexCoord2f( maxX * WATER_SCALE + Water::TEX_BIAS, maxY * WATER_SCALE + Water::TEX_BIAS );
        glVertex3f( v1.x, v1.y, 0.0f );

        glTexCoord2f( minX * WATER_SCALE + Water::TEX_BIAS, maxY * WATER_SCALE + Water::TEX_BIAS );
        glVertex3f( v0.x, v1.y, 0.0f );
      glEnd();
    }
    else {
      glNormal3f( 0.0f, 0.0f, -1.0f );

      glBegin( GL_QUADS );
        glColor4f( 1.0f, 1.0f, 1.0f, water.alpha1 );

        glTexCoord2f( minX * WATER_SCALE, maxY * WATER_SCALE );
        glVertex3f( v0.x, v1.y, 0.0f );

        glTexCoord2f( maxX * WATER_SCALE, maxY * WATER_SCALE );
        glVertex3f( v1.x, v1.y, 0.0f );

        glTexCoord2f( maxX * WATER_SCALE, minY * WATER_SCALE );
        glVertex3f( v1.x, v0.y, 0.0f );

        glTexCoord2f( minX * WATER_SCALE, minY * WATER_SCALE );
        glVertex3f( v0.x, v0.y, 0.0f );

        glColor4f( 1.0f, 1.0f, 1.0f, water.alpha2 );

        glTexCoord2f( minX * WATER_SCALE + Water::TEX_BIAS, maxY * WATER_SCALE + Water::TEX_BIAS );
        glVertex3f( v0.x, v1.y, 0.0f );

        glTexCoord2f( maxX * WATER_SCALE + Water::TEX_BIAS, maxY * WATER_SCALE + Water::TEX_BIAS );
        glVertex3f( v1.x, v1.y, 0.0f );

        glTexCoord2f( maxX * WATER_SCALE + Water::TEX_BIAS, minY * WATER_SCALE + Water::TEX_BIAS );
        glVertex3f( v1.x, v0.y, 0.0f );

        glTexCoord2f( minX * WATER_SCALE + Water::TEX_BIAS, minY * WATER_SCALE + Water::TEX_BIAS );
        glVertex3f( v0.x, v0.y, 0.0f );
      glEnd();
    }

    glDisable( GL_BLEND );
  }

}
}
