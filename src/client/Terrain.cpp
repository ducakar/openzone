/*
 *  Terrain.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "precompiled.h"

#include "Terrain.h"

#include "matrix/World.h"
#include "matrix/Terrain.h"

#include "Camera.h"
#include "Context.h"
#include "Frustum.h"
#include "Colors.h"
#include "Water.h"

#include <GL/glext.h>

#ifdef OZ_MINGW32
static PFNGLACTIVETEXTUREPROC       glActiveTexture       = null;
static PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture = null;
#endif

namespace oz
{
namespace client
{

  const float Terrain::DETAIL_SCALE = 4.0f;
  const float Terrain::WATER_SCALE  = 2.0f;

  void Terrain::load()
  {
#ifdef OZ_MINGW32
    glActiveTexture       = reinterpret_cast<PFNGLACTIVETEXTUREPROC>( SDL_GL_GetProcAddress( "glActiveTexture" ) );
    glClientActiveTexture = reinterpret_cast<PFNGLCLIENTACTIVETEXTUREPROC>( SDL_GL_GetProcAddress( "glClientActiveTexture" ) );

    detailTexId = context.loadTexture( "terra/detail.png" );
    mapTexId    = context.loadTexture( "terra/map.png" );
    waterTexId  = context.loadTexture( "terra/water.png" );
#else
    detailTexId = context.loadTexture( "terra/detail.jpg" );
    mapTexId    = context.loadTexture( "terra/map.png" );
    waterTexId  = context.loadTexture( "terra/water.jpg" );
#endif

    int nVertices = oz::Terrain::MAX * oz::Terrain::MAX;

    normals         = new Vec3[nVertices];
    detailTexCoords = new TexCoord[nVertices];
    mapTexCoords    = new TexCoord[nVertices];

    for( int x = 0; x < oz::Terrain::MAX; x++ ) {
      for( int y = 0; y < oz::Terrain::MAX; y++ ) {
        Vec3& n = normals[x * oz::Terrain::MAX + y];

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

        detailTexCoords[x * oz::Terrain::MAX + y].u = float( x & 1 ) * DETAIL_SCALE;
        detailTexCoords[x * oz::Terrain::MAX + y].v = float( y & 1 ) * DETAIL_SCALE;

        mapTexCoords[x * oz::Terrain::MAX + y].u = float( x ) / oz::Terrain::MAX;
        mapTexCoords[x * oz::Terrain::MAX + y].v = float( y ) / oz::Terrain::MAX;
      }
    }
  }

  void Terrain::unload()
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
    Area area;
    world.terra.getInters( area, camera.p.x - radius, camera.p.y - radius,
                           camera.p.x + radius, camera.p.y + radius );
    area.maxX++;
    area.maxY++;

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

    for( int y = area.minY; y < area.maxY; y++ ) {
      glBegin( GL_TRIANGLE_STRIP );
      for( int x = area.minX; x <= area.maxX; x++ ) {
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
    Area area;
    world.terra.getInters( area, camera.p.x - radius, camera.p.y - radius,
                           camera.p.x + radius, camera.p.y + radius );
    area.maxX++;
    area.maxY++;

    glBindTexture( GL_TEXTURE_2D, waterTexId );

    const Vec3& v0 = world.terra.vertices[area.minX][area.minY];
    const Vec3& v1 = world.terra.vertices[area.maxX][area.maxY];

    if( camera.p.z >= 0 ) {
      glNormal3f( 0.0f, 0.0f, 1.0f );

      glBegin( GL_QUADS );
        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colors::waterBlend1 );

        glTexCoord2f( area.minX * WATER_SCALE, area.minY * WATER_SCALE );
        glVertex3f( v0.x, v0.y, 0.0f );

        glTexCoord2f( area.maxX * WATER_SCALE, area.minY * WATER_SCALE );
        glVertex3f( v1.x, v0.y, 0.0f );

        glTexCoord2f( area.maxX * WATER_SCALE, area.maxY * WATER_SCALE );
        glVertex3f( v1.x, v1.y, 0.0f );

        glTexCoord2f( area.minX * WATER_SCALE, area.maxY * WATER_SCALE );
        glVertex3f( v0.x, v1.y, 0.0f );

        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colors::waterBlend2 );

        glTexCoord2f( area.minX * WATER_SCALE + Water::TEX_BIAS, area.minY * WATER_SCALE + Water::TEX_BIAS );
        glVertex3f( v0.x, v0.y, 0.0f );

        glTexCoord2f( area.maxX * WATER_SCALE + Water::TEX_BIAS, area.minY * WATER_SCALE + Water::TEX_BIAS );
        glVertex3f( v1.x, v0.y, 0.0f );

        glTexCoord2f( area.maxX * WATER_SCALE + Water::TEX_BIAS, area.maxY * WATER_SCALE + Water::TEX_BIAS );
        glVertex3f( v1.x, v1.y, 0.0f );

        glTexCoord2f( area.minX * WATER_SCALE + Water::TEX_BIAS, area.maxY * WATER_SCALE + Water::TEX_BIAS );
        glVertex3f( v0.x, v1.y, 0.0f );
      glEnd();
    }
    else {
      glNormal3f( 0.0f, 0.0f, -1.0f );

      glBegin( GL_QUADS );
        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colors::waterBlend1 );

        glTexCoord2f( area.minX * WATER_SCALE, area.maxY * WATER_SCALE );
        glVertex3f( v0.x, v1.y, 0.0f );

        glTexCoord2f( area.maxX * WATER_SCALE, area.maxY * WATER_SCALE );
        glVertex3f( v1.x, v1.y, 0.0f );

        glTexCoord2f( area.maxX * WATER_SCALE, area.minY * WATER_SCALE );
        glVertex3f( v1.x, v0.y, 0.0f );

        glTexCoord2f( area.minX * WATER_SCALE, area.minY * WATER_SCALE );
        glVertex3f( v0.x, v0.y, 0.0f );

        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colors::waterBlend2 );

        glTexCoord2f( area.minX * WATER_SCALE + Water::TEX_BIAS, area.maxY * WATER_SCALE + Water::TEX_BIAS );
        glVertex3f( v0.x, v1.y, 0.0f );

        glTexCoord2f( area.maxX * WATER_SCALE + Water::TEX_BIAS, area.maxY * WATER_SCALE + Water::TEX_BIAS );
        glVertex3f( v1.x, v1.y, 0.0f );

        glTexCoord2f( area.maxX * WATER_SCALE + Water::TEX_BIAS, area.minY * WATER_SCALE + Water::TEX_BIAS );
        glVertex3f( v1.x, v0.y, 0.0f );

        glTexCoord2f( area.minX * WATER_SCALE + Water::TEX_BIAS, area.minY * WATER_SCALE + Water::TEX_BIAS );
        glVertex3f( v0.x, v0.y, 0.0f );
      glEnd();
    }
  }

}
}
