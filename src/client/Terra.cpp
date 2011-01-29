/*
 *  Terra.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
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

  Terra terra;

  void Terra::prebuild()
  {
    int nVertices = oz::Terra::VERTS * oz::Terra::VERTS;
    int nIndices  = oz::Terra::VERTS * ( oz::Terra::VERTS - 1 ) * 2;

    DArray<VertexData> arrayData( nVertices );
    DArray<uint>       indexData( nIndices );

    String path = "terra/" + orbis.terra.name + ".ozcTerra";
    genBufferData( &arrayData, &indexData );
    saveBufferData( path, &arrayData, &indexData );
  }

  void Terra::load()
  {
    detailTexId = context.loadTexture( orbis.terra.detailTexture );
    mapTexId    = context.loadTexture( orbis.terra.mapTexture );
    waterTexId  = context.loadTexture( orbis.terra.waterTexture );

    int nVertices = oz::Terra::VERTS * oz::Terra::VERTS;
    int nIndices  = oz::Terra::VERTS * ( oz::Terra::VERTS - 1 ) * 2;

    DArray<VertexData> arrayData( nVertices );
    DArray<uint>       indexData( nIndices );

    String path = "terra/" + orbis.terra.name + ".ozcTerra";
    loadBufferData( path, &arrayData, &indexData );

    glGenBuffers( 1, &arrayBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, arrayBuffer );
    glBufferData( GL_ARRAY_BUFFER, nVertices * sizeof( VertexData ), arrayData, GL_STATIC_DRAW );

    glGenBuffers( 1, &indexBuffer );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof( uint ), indexData, GL_STATIC_DRAW );
  }

  void Terra::genBufferData( DArray<VertexData>* arrayData, DArray<uint>* indexData )
  {
    log.print( "Generating client terrain data ..." );

    for( int x = 0; x < oz::Terra::VERTS; ++x ) {
      for( int y = 0; y < oz::Terra::VERTS; ++y ) {
        VertexData& vertex = ( *arrayData )[x * oz::Terra::VERTS + y];

        vertex.position = orbis.terra.quads[x][y].vertex;
        vertex.normal   = Vec3::ZERO;

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
        vertex.normal = ~vertex.normal;

        vertex.detailTexCoord.u = float( x & 1 ) * DETAIL_SCALE;
        vertex.detailTexCoord.v = float( y & 1 ) * DETAIL_SCALE;

        vertex.mapTexCoord.u = float( x ) / oz::Terra::VERTS;
        vertex.mapTexCoord.v = float( y ) / oz::Terra::VERTS;

        if( x != oz::Terra::VERTS - 1 ) {
          ( *indexData )[ 2 * ( x * oz::Terra::VERTS + y ) ] = ( x + 1 ) * oz::Terra::VERTS + y;
          ( *indexData )[ 2 * ( x * oz::Terra::VERTS + y ) + 1 ] = x * oz::Terra::VERTS + y;
        }
      }
    }

    log.printEnd( " OK" );
  }

  void Terra::saveBufferData( const char* path, DArray<VertexData>* arrayData,
                              DArray<uint>* indexData )
  {
    log.print( "Saving client terrain data to '%s' ...", path );

    int size = 0;
    size += int( 2 * sizeof( int ) );
    size += int( arrayData->length() * sizeof( VertexData ) );
    size += int( indexData->length() * sizeof( uint ) );

    Buffer buffer( size );
    OutputStream os = buffer.outputStream();

    os.writeInt( arrayData->length() );
    os.writeInt( indexData->length() );

    for( int i = 0; i < arrayData->length(); ++i ) {
      const VertexData& vertex = ( *arrayData )[i];

      os.writePoint3( vertex.position );
      os.writeVec3( vertex.normal );
      os.writeFloat( vertex.detailTexCoord.u );
      os.writeFloat( vertex.detailTexCoord.v );
      os.writeFloat( vertex.mapTexCoord.u );
      os.writeFloat( vertex.mapTexCoord.v );
    }

    for( int i = 0; i < indexData->length(); ++i ) {
      const uint& index = ( *indexData )[i];

      os.writeInt( index );
    }

    buffer.write( path );

    log.printEnd( " OK" );
  }

  void Terra::loadBufferData( const char* path, DArray<VertexData>* arrayData,
                              DArray<uint>* indexData )
  {
    log.print( "Loading client terrain data from '%s' ...", path );

    Buffer buffer;
    if( !buffer.read( path ) ) {
      throw Exception( "Cannot read terrain file" );
    }
    InputStream is = buffer.inputStream();

    int nVertices = is.readInt();
    int nIndices = is.readInt();

    if( nVertices != arrayData->length() || nIndices != indexData->length() ) {
      throw Exception( "Invalid client terrain size" );
    }

    for( int i = 0; i < nVertices; ++i ) {
      VertexData& vertex = ( *arrayData )[i];

      vertex.position = is.readPoint3();
      vertex.normal = is.readVec3();
      vertex.detailTexCoord.u = is.readFloat();
      vertex.detailTexCoord.v = is.readFloat();
      vertex.mapTexCoord.u = is.readFloat();
      vertex.mapTexCoord.v = is.readFloat();
    }

    for( int i = 0; i < nIndices; ++i ) {
      uint& index = ( *indexData )[i];

      index = is.readInt();
    }

    log.printEnd( " OK" );
  }

  void Terra::unload()
  {
    glDeleteBuffers( 1, &indexBuffer );
    glDeleteBuffers( 1, &arrayBuffer );

    context.freeTexture( waterTexId );
    context.freeTexture( mapTexId );
    context.freeTexture( detailTexId );
  }

  void Terra::draw() const
  {
    Span span = orbis.terra.getInters( camera.p.x - radius, camera.p.y - radius,
                                       camera.p.x + radius, camera.p.y + radius );
    ++span.maxX;
    ++span.maxY;

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

    glBindBuffer( GL_ARRAY_BUFFER, arrayBuffer );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer );

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
      int offset = ( x * oz::Terra::VERTS + span.minY ) * 2;
      glDrawElements( GL_TRIANGLE_STRIP, count, GL_UNSIGNED_INT, OZ_VBO_OFFSET( offset, uint ) );
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );
    glDisableClientState( GL_VERTEX_ARRAY );

    glActiveTexture( GL_TEXTURE1 );
    glDisable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE0 );

    glFrontFace( GL_CCW );
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

    const Point3& v0 = orbis.terra.quads[span.minX][span.minY].vertex;
    const Point3& v1 = orbis.terra.quads[span.maxX][span.maxY].vertex;

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
