/*
 *  Terra.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Terra.hpp"

#include "matrix/Orbis.hpp"
#include "matrix/Terra.hpp"

#include "client/Camera.hpp"
#include "client/Context.hpp"
#include "client/Frustum.hpp"
#include "client/Colours.hpp"
#include "client/Water.hpp"

namespace oz
{
namespace client
{

  const float Terra::DETAIL_SCALE = 4.0f;
  const float Terra::WATER_SCALE  = 2.0f;

  struct TexCoord
  {
    float u;
    float v;
  };

  struct VertexData
  {
    Vec3     position;
    Vec3     normal;
    TexCoord detailTexCoord;
    TexCoord mapTexCoord;
  };

  Terra terra;

  void Terra::load()
  {
    detailTexId = context.loadTexture( orbis.terra.detailTexture );
    mapTexId    = context.loadTexture( orbis.terra.mapTexture );
    waterTexId  = context.loadTexture( orbis.terra.waterTexture );

    int nVertices = oz::Terra::MAX * oz::Terra::MAX;
    int nIndices = oz::Terra::MAX * ( oz::Terra::MAX - 1 ) * 2;

    VertexData* arrayData = new VertexData[nVertices];
    uint* indexData = new uint[nIndices];

    for( int x = 0; x < oz::Terra::MAX; ++x ) {
      for( int y = 0; y < oz::Terra::MAX; ++y ) {
        VertexData& vertex = arrayData[x * oz::Terra::MAX + y];

        vertex.position = orbis.terra.vertices[x][y];

        vertex.normal.setZero();
        if( x < oz::Terra::QUADS && y < oz::Terra::QUADS ) {
          vertex.normal += orbis.terra.quads[x][y].tri[0].normal;
          vertex.normal += orbis.terra.quads[x][y].tri[1].normal;
        }
        if( x > 0 && y < oz::Terra::QUADS ) {
          vertex.normal += orbis.terra.quads[x - 1][y].tri[0].normal;
        }
        if( x > 0 && y > 0 ) {
          vertex.normal += orbis.terra.quads[x - 1][y - 1].tri[0].normal;
          vertex.normal += orbis.terra.quads[x - 1][y - 1].tri[1].normal;
        }
        if( x < oz::Terra::QUADS && y > 0 ) {
          vertex.normal += orbis.terra.quads[x][y - 1].tri[1].normal;
        }
        vertex.normal.norm();

        vertex.detailTexCoord.u = float( x & 1 ) * DETAIL_SCALE;
        vertex.detailTexCoord.v = float( y & 1 ) * DETAIL_SCALE;

        vertex.mapTexCoord.u = float( x ) / oz::Terra::MAX;
        vertex.mapTexCoord.v = float( y ) / oz::Terra::MAX;

        if( x != oz::Terra::MAX - 1 ) {
          indexData[ 2 * ( x * oz::Terra::MAX + y ) ] = ( x + 1 ) * oz::Terra::MAX + y;
          indexData[ 2 * ( x * oz::Terra::MAX + y ) + 1 ] = x * oz::Terra::MAX + y;
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

  void Terra::unload()
  {
    glDeleteBuffers( 1, &indexBuffer );
    glDeleteBuffers( 1, &arrayBuffer );
  }

  void Terra::draw() const
  {
    Span span = orbis.terra.getInters( camera.p.x - radius, camera.p.y - radius,
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
      int offset = ( x * oz::Terra::MAX + span.minY ) * 2;
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

  void Terra::drawWater() const
  {
    Span span = orbis.terra.getInters( camera.p.x - radius, camera.p.y - radius,
                                       camera.p.x + radius, camera.p.y + radius );
    ++span.maxX;
    ++span.maxY;

    float minX = float( span.minX );
    float maxX = float( span.maxX );
    float minY = float( span.minY );
    float maxY = float( span.maxY );

    glBindTexture( GL_TEXTURE_2D, waterTexId );

    const Vec3& v0 = orbis.terra.vertices[span.minX][span.minY];
    const Vec3& v1 = orbis.terra.vertices[span.maxX][span.maxY];

    if( camera.p.z >= 0 ) {
      glNormal3f( 0.0f, 0.0f, 1.0f );

      glBegin( GL_QUADS );
        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colours::waterBlend1 );

        glTexCoord2f( minX * WATER_SCALE, minY * WATER_SCALE );
        glVertex3f( v0.x, v0.y, 0.0f );

        glTexCoord2f( maxX * WATER_SCALE, minY * WATER_SCALE );
        glVertex3f( v1.x, v0.y, 0.0f );

        glTexCoord2f( maxX * WATER_SCALE, maxY * WATER_SCALE );
        glVertex3f( v1.x, v1.y, 0.0f );

        glTexCoord2f( minX * WATER_SCALE, maxY * WATER_SCALE );
        glVertex3f( v0.x, v1.y, 0.0f );

        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colours::waterBlend2 );

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
        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colours::waterBlend1 );

        glTexCoord2f( minX * WATER_SCALE, maxY * WATER_SCALE );
        glVertex3f( v0.x, v1.y, 0.0f );

        glTexCoord2f( maxX * WATER_SCALE, maxY * WATER_SCALE );
        glVertex3f( v1.x, v1.y, 0.0f );

        glTexCoord2f( maxX * WATER_SCALE, minY * WATER_SCALE );
        glVertex3f( v1.x, v0.y, 0.0f );

        glTexCoord2f( minX * WATER_SCALE, minY * WATER_SCALE );
        glVertex3f( v0.x, v0.y, 0.0f );

        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colours::waterBlend2 );

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
