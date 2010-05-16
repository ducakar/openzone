/*
 *  Terrain.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "stable.h"

#include "client/Terrain.h"

#include "matrix/World.h"
#include "matrix/Terrain.h"

#include "client/Camera.h"
#include "client/Context.h"
#include "client/Frustum.h"
#include "client/Colors.h"
#include "client/Water.h"

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
    int nIndices = oz::Terrain::MAX * ( oz::Terrain::MAX - 1 ) * 2;

    VertexData* arrayData = new VertexData[nVertices];
    uint* indexData = new uint[nIndices];

    for( int x = 0; x < oz::Terrain::MAX; ++x ) {
      for( int y = 0; y < oz::Terrain::MAX; ++y ) {
        VertexData& vertex = arrayData[x * oz::Terrain::MAX + y];

        vertex.position = world.terra.vertices[x][y];

        vertex.normal.setZero();
        if( x < oz::Terrain::QUADS && y < oz::Terrain::QUADS ) {
          vertex.normal += world.terra.quads[x][y].tri[0].normal;
          vertex.normal += world.terra.quads[x][y].tri[1].normal;
        }
        if( x > 0 && y < oz::Terrain::QUADS ) {
          vertex.normal += world.terra.quads[x - 1][y].tri[0].normal;
        }
        if( x > 0 && y > 0 ) {
          vertex.normal += world.terra.quads[x - 1][y - 1].tri[0].normal;
          vertex.normal += world.terra.quads[x - 1][y - 1].tri[1].normal;
        }
        if( x < oz::Terrain::QUADS && y > 0 ) {
          vertex.normal += world.terra.quads[x][y - 1].tri[1].normal;
        }
        vertex.normal.norm();

        vertex.detailTexCoord.u = float( x & 1 ) * DETAIL_SCALE;
        vertex.detailTexCoord.v = float( y & 1 ) * DETAIL_SCALE;

        vertex.mapTexCoord.u = float( x ) / oz::Terrain::MAX;
        vertex.mapTexCoord.v = float( y ) / oz::Terrain::MAX;

        if( x != oz::Terrain::MAX - 1 ) {
          indexData[ 2 * ( x * oz::Terrain::MAX + y ) ] = ( x + 1 ) * oz::Terrain::MAX + y;
          indexData[ 2 * ( x * oz::Terrain::MAX + y ) + 1 ] = x * oz::Terrain::MAX + y;
        }
      }
    }

    glGenBuffers( 1, &arrayBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, arrayBuffer );
    glBufferData( GL_ARRAY_BUFFER, nVertices * sizeof( VertexData ), arrayData, GL_STATIC_DRAW );

    glGenBuffers( 1, &indexBuffer );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof( uint ), indexData, GL_STATIC_DRAW );

    delete[] indexData;
    delete[] arrayData;
  }

  void Terrain::unload()
  {
    glDeleteBuffers( 1, &indexBuffer );
    glDeleteBuffers( 1, &arrayBuffer );
  }

  void Terrain::draw() const
  {
    Span span;
    world.terra.getInters( span, camera.p.x - radius, camera.p.y - radius,
                           camera.p.x + radius, camera.p.y + radius );
    ++span.maxX;
    ++span.maxY;

    glBindBuffer( GL_ARRAY_BUFFER, arrayBuffer );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer );

    // to match strip triangles with matrix terrain we have to make them clockwise since
    // we draw column-major (strips along y axis) for better cache performance
    glFrontFace( GL_CW );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, detailTexId );

    glActiveTexture( GL_TEXTURE1 );
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, mapTexId );

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    glVertexPointer( 3, GL_FLOAT, sizeof( VertexData ),
                     OZ_VBO_OFFSETOF( 0, VertexData, position ) );
    glNormalPointer( GL_FLOAT, sizeof( VertexData ),
                     OZ_VBO_OFFSETOF( 0, VertexData, normal ) );

    glClientActiveTexture( GL_TEXTURE0 );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( VertexData ),
                       OZ_VBO_OFFSETOF( 0, VertexData, detailTexCoord ) );

    glClientActiveTexture( GL_TEXTURE1 );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( VertexData ),
                       OZ_VBO_OFFSETOF( 0, VertexData, mapTexCoord ) );

    int count = ( span.maxY - span.minY + 1 ) * 2;
    for( int x = span.minX; x < span.maxX; ++x ) {
      int offset = ( x * oz::Terrain::MAX + span.minY ) * 2;
      glDrawElements( GL_TRIANGLE_STRIP, count, GL_UNSIGNED_INT, OZ_VBO_OFFSET( offset, uint ) );
    }

    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );
    glDisableClientState( GL_VERTEX_ARRAY );

    glActiveTexture( GL_TEXTURE1 );
    glDisable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE0 );

    glFrontFace( GL_CCW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
  }

  void Terrain::drawWater() const
  {
    Span span;
    world.terra.getInters( span, camera.p.x - radius, camera.p.y - radius,
                           camera.p.x + radius, camera.p.y + radius );
    ++span.maxX;
    ++span.maxY;

    float minX = float( span.minX );
    float maxX = float( span.maxX );
    float minY = float( span.minY );
    float maxY = float( span.maxY );

    glBindTexture( GL_TEXTURE_2D, waterTexId );

    const Vec3& v0 = world.terra.vertices[span.minX][span.minY];
    const Vec3& v1 = world.terra.vertices[span.maxX][span.maxY];

    if( camera.p.z >= 0 ) {
      glNormal3f( 0.0f, 0.0f, 1.0f );

      glBegin( GL_QUADS );
        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colors::waterBlend1 );

        glTexCoord2f( minX * WATER_SCALE, minY * WATER_SCALE );
        glVertex3f( v0.x, v0.y, 0.0f );

        glTexCoord2f( maxX * WATER_SCALE, minY * WATER_SCALE );
        glVertex3f( v1.x, v0.y, 0.0f );

        glTexCoord2f( maxX * WATER_SCALE, maxY * WATER_SCALE );
        glVertex3f( v1.x, v1.y, 0.0f );

        glTexCoord2f( minX * WATER_SCALE, maxY * WATER_SCALE );
        glVertex3f( v0.x, v1.y, 0.0f );

        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colors::waterBlend2 );

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
        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colors::waterBlend1 );

        glTexCoord2f( minX * WATER_SCALE, maxY * WATER_SCALE );
        glVertex3f( v0.x, v1.y, 0.0f );

        glTexCoord2f( maxX * WATER_SCALE, maxY * WATER_SCALE );
        glVertex3f( v1.x, v1.y, 0.0f );

        glTexCoord2f( maxX * WATER_SCALE, minY * WATER_SCALE );
        glVertex3f( v1.x, v0.y, 0.0f );

        glTexCoord2f( minX * WATER_SCALE, minY * WATER_SCALE );
        glVertex3f( v0.x, v0.y, 0.0f );

        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colors::waterBlend2 );

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
  }

}
}
