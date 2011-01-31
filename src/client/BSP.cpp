/*
 *  BSP.cpp
 *
 *  BSP level rendering class
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/BSP.hpp"

#include "client/Context.hpp"
#include "client/Frustum.hpp"
#include "client/Colours.hpp"
#include "client/Water.hpp"
#include "client/Render.hpp"

#include <SDL_opengl.h>

namespace oz
{
namespace client
{

  static const int QBSP_SLICK_BIT    = 0x00000002;
  static const int QBSP_LADDER_BIT   = 0x00000008;
  static const int QBSP_WATER_BIT    = 0x00000020;
  static const int QBSP_NONSOLID_BIT = 0x00004000;

  struct QBSPHeader
  {
    char id[4];
    int  version;
  };

  struct QBSPLump
  {
    int offset;
    int length;
  };

  enum QBSPLumpType
  {
    QBSP_LUMP_ENTITIES = 0,
    QBSP_LUMP_TEXTURES,
    QBSP_LUMP_PLANES,
    QBSP_LUMP_NODES,
    QBSP_LUMP_LEAFS,
    QBSP_LUMP_LEAFFACES,
    QBSP_LUMP_LEAFBRUSHES,
    QBSP_LUMP_MODELS,
    QBSP_LUMP_BRUSHES,
    QBSP_LUMP_BRUSHSIDES,
    QBSP_LUMP_VERTICES,
    QBSP_LUMP_INDICES,
    QBSP_LUMP_SHADERS,
    QBSP_LUMP_FACES,
    QBSP_LUMP_LIGHTMAPS,
    QBSP_LUMP_LIGHTVOLUMES,
    QBSP_LUMP_VISUALDATA,
    QBSP_LUMPS_NUM
  };

  struct QBSPTexture
  {
    char name[64];
    int  flags;
    int  type;
  };

  struct QBSPPlane
  {
    float normal[3];
    float distance;
  };

  struct QBSPNode
  {
    int plane;

    int front;
    int back;

    int bb[2][3];
  };

  struct QBSPLeaf
  {
    int cluster;
    int area;

    int bb[2][3];

    int firstFace;
    int nFaces;

    int firstBrush;
    int nBrushes;
  };

  struct QBSPModel
  {
    float bb[2][3];

    int firstFace;
    int nFaces;

    int firstBrush;
    int nBrushes;
  };

  struct QBSPVertex
  {
    float p[3];
    float texCoord[2];
    float lightmapCoord[2];
    float normal[3];
    char  colour[4];
  };

  struct QBSPFace
  {
    int   texture;
    int   effect;
    int   type;

    int   firstVertex;
    int   nVertices;

    int   firstIndex;
    int   nIndices;

    int   lightmap;
    int   lightmapCorner[2];
    int   lightmapSize[2];

    float lightmapPos[3];
    float lightmapVecs[2][3];

    float normal[3];

    int   size[2];
  };

  const Struct* BSP::str;
  Point3 BSP::camPos;
  int BSP::waterFlags;

  BSP::VisualData::VisualData() : bitsets( null )
  {}

  BSP::VisualData::~VisualData()
  {
    if( bitsets != null ) {
      delete[] bitsets;
    }
  }

  inline Bounds BSP::rotateBounds( const Bounds& bounds, Struct::Rotation rotation )
  {
    Bounds rotatedBounds;

    switch( rotation ) {
      case Struct::R0: {
        return bounds;
      }
      case Struct::R90: {
        return Bounds( Point3( -bounds.maxs.y, bounds.mins.x, bounds.mins.z ),
                       Point3( -bounds.mins.y, bounds.maxs.x, bounds.maxs.z ) );
      }
      case Struct::R180: {
        return Bounds( Point3( -bounds.maxs.x, -bounds.maxs.y, bounds.mins.z ),
                       Point3( -bounds.mins.x, -bounds.mins.y, bounds.maxs.z ) );
      }
      default:
      case Struct::R270: {
        return Bounds( Point3( bounds.mins.y, -bounds.maxs.x, bounds.mins.z ),
                       Point3( bounds.maxs.y, -bounds.mins.x, bounds.maxs.z ) );
      }
    }
  }

  int BSP::getLeaf() const
  {
    int nodeIndex = 0;
    do {
      const Node&  node  = nodes[nodeIndex];
      const Plane& plane = planes[node.plane];

      if( ( camPos * plane.normal - plane.distance ) < 0.0f ) {
        nodeIndex = node.back;
      }
      else {
        nodeIndex = node.front;
      }

      assert( nodeIndex != 0 );
    }
    while( nodeIndex >= 0 );

    return ~nodeIndex;
  }

  bool BSP::isInWater() const
  {
    int nodeIndex = 0;
    do {
      const oz::BSP::Node&  node  = bsp->nodes[nodeIndex];
      const oz::BSP::Plane& plane = bsp->planes[node.plane];

      if( ( camPos * plane.normal - plane.distance ) < 0.0f ) {
        nodeIndex = node.back;
      }
      else {
        nodeIndex = node.front;
      }

      assert( nodeIndex != 0 );
    }
    while( nodeIndex >= 0 );

    const oz::BSP::Leaf& leaf = bsp->leaves[~nodeIndex];

    for( int i = 0; i < leaf.nBrushes; ++i ) {
      const oz::BSP::Brush* brush = &bsp->brushes[ bsp->leafBrushes[leaf.firstBrush + i] ];

      if( brush->material & Material::WATER_BIT ) {
        for( int i = 0; i < brush->nSides; ++i ) {
          const oz::BSP::Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

          if( ( camPos * plane.normal - plane.distance ) >= 0.0f ) {
            goto nextBrush;
          }
        }
        return true;
      }
      nextBrush:;
    }
    return false;
  }

  void BSP::drawFace( const Face* face ) const
  {
    if( face->material & Material::WATER_BIT ) {
      waterFlags |= DRAW_WATER;
      return;
    }

    glVertexPointer( 3, GL_FLOAT, sizeof( Vertex ), vertices[face->firstVertex].p );

    glBindTexture( GL_TEXTURE_2D, texIds[face->texture] );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ), vertices[face->firstVertex].texCoord );

    if( nLightmaps != 0 ) {
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture( GL_TEXTURE_2D, lightmapIds[face->lightmap] );

      glClientActiveTexture( GL_TEXTURE1 );
      glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ), vertices[face->firstVertex].lightmapCoord );

      glActiveTexture( GL_TEXTURE0 );
      glClientActiveTexture( GL_TEXTURE0 );
    }

    glNormal3fv( face->normal );
    glDrawElements( GL_TRIANGLES, face->nIndices, GL_UNSIGNED_INT, &indices[face->firstIndex] );
  }

  void BSP::drawFaceWater( const Face* face ) const
  {
    glVertexPointer( 3, GL_FLOAT, sizeof( Vertex ), vertices[face->firstVertex].p );

    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colours::waterBlend1 );
    glBindTexture( GL_TEXTURE_2D, texIds[face->texture] );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ), vertices[face->firstVertex].texCoord );

    if( nLightmaps != 0 ) {
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture( GL_TEXTURE_2D, lightmapIds[face->lightmap] );

      glClientActiveTexture( GL_TEXTURE1 );
      glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ), vertices[face->firstVertex].lightmapCoord );

      glActiveTexture( GL_TEXTURE0 );
      glClientActiveTexture( GL_TEXTURE0 );
    }

    glNormal3f( face->normal.x, face->normal.y, face->normal.z );
    glDrawElements( GL_TRIANGLES, face->nIndices, GL_UNSIGNED_INT, &indices[face->firstIndex] );

    glFrontFace( GL_CCW );
    glNormal3f( -face->normal.x, -face->normal.y, -face->normal.z );
    glDrawElements( GL_TRIANGLES, face->nIndices, GL_UNSIGNED_INT, &indices[face->firstIndex] );

    glMatrixMode( GL_TEXTURE );
    glLoadMatrixf( Mat44( 1.0f,            0.0f,            0.0f, 0.0f,
                          0.0f,            1.0f,            0.0f, 0.0f,
                          0.0f,            0.0f,            1.0f, 0.0f,
                          Water::TEX_BIAS, Water::TEX_BIAS, 0.0f, 1.0f ) );

    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colours::waterBlend2 );
    glBindTexture( GL_TEXTURE_2D, texIds[face->texture] );

    glDrawElements( GL_TRIANGLES, face->nIndices, GL_UNSIGNED_INT, &indices[face->firstIndex] );

    glFrontFace( GL_CW );
    glNormal3f( face->normal.x, face->normal.y, face->normal.z );
    glDrawElements( GL_TRIANGLES, face->nIndices, GL_UNSIGNED_INT, &indices[face->firstIndex] );

    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
  }

  void BSP::drawNode( int nodeIndex )
  {
    if( nodeIndex >= 0 ) {
      const Node&  node  = nodes[nodeIndex];
      const Plane& plane = planes[node.plane];

      if( ( camPos * plane.normal - plane.distance ) < 0.0f ) {
        drawNode( node.back );
        drawNode( node.front );
      }
      else {
        drawNode( node.front );
        drawNode( node.back );
      }
    }
    else {
      const Leaf& leaf = leaves[~nodeIndex];
      Bounds rotatedLeaf = rotateBounds( leaf, str->rot );

      if( frustum.isVisible( leaf + ( str->p - Point3::ORIGIN ) ) ) {
        for( int i = 0; i < leaf.nFaces; ++i ) {
          int face = leafFaces[leaf.firstFace + i];

          if( !drawnFaces.get( face ) ) {
            drawFace( &faces[face] );
            drawnFaces.set( face );
          }
        }
      }
    }
  }

  void BSP::drawNodeWater( int nodeIndex )
  {
    if( nodeIndex >= 0 ) {
      const Node&  node  = nodes[nodeIndex];
      const Plane& plane = planes[node.plane];

      if( ( camPos * plane.normal - plane.distance ) < 0.0f ) {
        drawNodeWater( node.back );
        drawNodeWater( node.front );
      }
      else {
        drawNodeWater( node.front );
        drawNodeWater( node.back );
      }
    }
    else {
      const Leaf& leaf = leaves[~nodeIndex];
      Bounds rotatedLeaf = rotateBounds( leaf, str->rot );

      if( frustum.isVisible( leaf + ( str->p - Point3::ORIGIN ) ) ) {
        for( int i = 0; i < leaf.nFaces; ++i ) {
          int face = leafFaces[leaf.firstFace + i];

          if( !drawnFaces.get( face ) ) {
            drawFaceWater( &faces[face] );
            drawnFaces.set( face );
          }
        }
      }
    }
  }

  bool BSP::loadOZCBSP( const char* fileName )
  {
    Buffer buffer;
    buffer.read( fileName );

    if( buffer.isEmpty() ) {
      return false;
    }

    InputStream is = buffer.inputStream();

    mins          = is.readPoint3();
    maxs          = is.readPoint3();

    nTextures     = is.readInt();
    nPlanes       = is.readInt();
    nNodes        = is.readInt();
    nLeaves       = is.readInt();
    nEntityModels = is.readInt();
    nVertices     = is.readInt();
    nIndices      = is.readInt();
    nLeafFaces    = is.readInt();
    nFaces        = is.readInt();
    nLightmaps    = is.readInt();

    int size = 0;

    size += nTextures     * int( 64 * sizeof( char ) );
    size = Alloc::alignUp( size );

    size += nPlanes       * int( sizeof( Plane ) );
    size = Alloc::alignUp( size );

    size += nNodes        * int( sizeof( Node ) );
    size = Alloc::alignUp( size );

    size += nLeaves       * int( sizeof( Leaf ) );
    size = Alloc::alignUp( size );

    size += nEntityModels * int( sizeof( EntityModel ) );
    size = Alloc::alignUp( size );

    size += nVertices     * int( sizeof( Vertex ) );
    size = Alloc::alignUp( size );

    size += nIndices      * int( sizeof( int ) );
    size = Alloc::alignUp( size );

    size += nLeafFaces    * int( sizeof( int ) );
    size = Alloc::alignUp( size );

    size += nFaces        * int( sizeof( Face ) );
    size = Alloc::alignUp( size );

    size += nLightmaps    * int( sizeof( Lightmap ) );
    size = Alloc::alignUp( size );

    char* data = Alloc::alloc<char>( size );

    textures = new( data ) int[nTextures];
    for( int i = 0; i < nTextures; ++i ) {
      String textureName = is.readPaddedString( 64 );

      if( textureName.isEmpty() ) {
        textures[i] = -1;
      }
      else {
        textures[i] = translator.textureIndex( textureName );
      }
    }
    data += nTextures * 64 * sizeof( char );
    data = Alloc::alignUp( data );

    planes = new( data ) Plane[nPlanes];
    for( int i = 0; i < nPlanes; ++i ) {
      planes[i].normal = is.readVec3();
      planes[i].distance = is.readFloat();
    }
    data += nPlanes * sizeof( Plane );
    data = Alloc::alignUp( data );

    nodes = new( data ) Node[nNodes];
    for( int i = 0; i < nNodes; ++i ) {
      nodes[i].plane = is.readInt();
      nodes[i].front = is.readInt();
      nodes[i].back = is.readInt();
    }
    data += nNodes * sizeof( Node );
    data = Alloc::alignUp( data );

    leaves = new( data ) Leaf[nLeaves];
    for( int i = 0; i < nLeaves; ++i ) {
      leaves[i].firstFace = is.readInt();
      leaves[i].nFaces = is.readInt();
      leaves[i].cluster = is.readInt();
    }
    data += nLeaves * sizeof( Leaf );
    data = Alloc::alignUp( data );

    entityModels = new( data ) EntityModel[nEntityModels];
    for( int i = 0; i < nEntityModels; ++i ) {
      entityModels[i].firstFace = is.readInt();
      entityModels[i].nFaces = is.readInt();
    }
    data += nEntityModels * sizeof( EntityModel );
    data = Alloc::alignUp( data );

    vertices = new( data ) Vertex[nVertices];
    for( int i = 0; i < nVertices; ++i ) {
      vertices[i].p = is.readPoint3();
      vertices[i].texCoord.u = is.readFloat();
      vertices[i].texCoord.v = is.readFloat();
      vertices[i].lightmapCoord.u = is.readFloat();
      vertices[i].lightmapCoord.v = is.readFloat();
    }
    data += nVertices * sizeof( Vertex );
    data = Alloc::alignUp( data );

    indices = new( data ) int[nIndices];
    for( int i = 0; i < nIndices; ++i ) {
      indices[i] = is.readInt();
    }
    data += nIndices * sizeof( int );
    data = Alloc::alignUp( data );

    leafFaces = new( data ) int[nLeafFaces];
    for( int i = 0; i < nLeafFaces; ++i ) {
      leafFaces[i] = is.readInt();
    }
    data += nLeafFaces * sizeof( int );
    data = Alloc::alignUp( data );

    faces = new( data ) Face[nFaces];
    for( int i = 0; i < nFaces; ++i ) {
      faces[i].normal = is.readVec3();
      faces[i].texture = is.readInt();
      faces[i].lightmap = is.readInt();
      faces[i].material = is.readInt();
      faces[i].firstVertex = is.readInt();
      faces[i].nVertices = is.readInt();
      faces[i].firstIndex = is.readInt();
      faces[i].nIndices = is.readInt();
    }
    data += nFaces * sizeof( Face );
    data = Alloc::alignUp( data );

    lightmaps = new( data ) Lightmap[nLightmaps];
    for( int i = 0; i < nLightmaps; ++i ) {
      is.readChars( lightmaps[i].bits, LIGHTMAP_SIZE );
    }

    visual.nClusters = 0;
    visual.clusterLength = 0;
    visual.bitsets = null;

    return true;
  }

  void BSP::freeOZCBSP()
  {
    log.print( "Freeing BSP model '%s' ...", name.cstr() );

    if( textures != null ) {
      aDestruct( textures, nTextures );
      aDestruct( planes, nPlanes );
      aDestruct( nodes, nNodes );
      aDestruct( leaves, nLeaves );
      aDestruct( entityModels, nEntityModels );
      aDestruct( vertices, nVertices );
      aDestruct( indices, nIndices );
      aDestruct( leafFaces, nLeafFaces );
      aDestruct( faces, nFaces );
      aDestruct( lightmaps, nLightmaps );

      Alloc::dealloc( textures );

      nTextures     = 0;
      nPlanes       = 0;
      nNodes        = 0;
      nLeaves       = 0;
      nEntityModels = 0;
      nVertices     = 0;
      nIndices      = 0;
      nLeafFaces    = 0;
      nFaces        = 0;
      nLightmaps    = 0;

      textures     = null;
      planes       = null;
      nodes        = null;
      leaves       = null;
      entityModels = null;
      vertices     = null;
      indices      = null;
      leafFaces    = null;
      faces        = null;
      lightmaps    = null;
    }

    log.printEnd( " OK" );
  }

  bool BSP::loadQBSP( const char* fileName )
  {
    String rcFile = fileName + String( ".rc" );
    String bspFile = fileName + String( ".bsp" );

    Config bspConfig;
    if( !bspConfig.load( rcFile ) ) {
      return false;
    }

    float scale = bspConfig.get( "scale", 0.01f );
    float maxDim = bspConfig.get( "maxDim", Math::inf() );

    if( Math::isNaN( scale ) ) {
      log.println( " Invalid config" );
      log.unindent();
      log.println( "}" );
      return false;
    }

    FILE* file = fopen( bspFile, "rb" );
    if( file == null ) {
      log.println( "File not found" );
      return false;
    }

    QBSPHeader header;
    fread( &header, sizeof( QBSPHeader ), 1, file );

    if( header.id[0] != 'I' || header.id[1] != 'B' || header.id[2] != 'S' || header.id[3] != 'P' ||
        header.version != 46 )
    {
      log.println( "Wrong format" );
      return false;
    }

    QBSPLump lumps[QBSP_LUMPS_NUM];
    fread( lumps, sizeof( QBSPLump ), QBSP_LUMPS_NUM, file );

    nTextures = int( lumps[QBSP_LUMP_TEXTURES].length / sizeof( QBSPTexture ) );
    textures = new int[nTextures];
    int* texFlags = new int[nTextures];
    int* texTypes = new int[nTextures];
    fseek( file, lumps[QBSP_LUMP_TEXTURES].offset, SEEK_SET );

    log.println( "Loading texture descriptions {" );
    log.indent();

    for( int i = 0; i < nTextures; ++i ) {
      QBSPTexture texture;

      fread( &texture, sizeof( QBSPTexture ), 1, file );

      String name = texture.name;
      texFlags[i] = texture.flags;
      texTypes[i] = texture.type;

      log.print( "%s", name.cstr() );

      if( name.length() <= 12 || name.equals( "textures/NULL" ) ||
          ( texture.flags & QBSP_LADDER_BIT ) )
      {
        textures[i] = -1;
        log.printEnd();
      }
      else {
        name = name.substring( 12 );
        textures[i] = translator.textureIndex( name );
        log.printEnd( " 0x%x 0x%x", texture.flags, texture.type );
      }
    }

    log.unindent();
    log.println( "}" );

    nPlanes = int( lumps[QBSP_LUMP_PLANES].length / sizeof( QBSPPlane ) );
    planes = new BSP::Plane[nPlanes];
    fseek( file, lumps[QBSP_LUMP_PLANES].offset, SEEK_SET );

    // rescale plane data
    for( int i = 0; i < nPlanes; ++i ) {
      QBSPPlane plane;

      fread( &plane, sizeof( QBSPPlane ), 1, file );

      planes[i].normal   = Vec3( plane.normal );
      planes[i].distance = plane.distance * scale;

      float offset =
          Math::abs( planes[i].normal.x * maxDim ) +
          Math::abs( planes[i].normal.y * maxDim ) +
          Math::abs( planes[i].normal.z * maxDim );

      if( planes[i].distance < -offset ) {
        planes[i].distance = -Math::inf();
      }
      else if( planes[i].distance > offset ) {
        planes[i].distance = Math::inf();
      }
    }

    nNodes = int( lumps[QBSP_LUMP_NODES].length / sizeof( QBSPNode ) );
    nodes = new BSP::Node[nNodes];
    fseek( file, lumps[QBSP_LUMP_NODES].offset, SEEK_SET );

    for( int i = 0; i < nNodes; ++i ) {
      QBSPNode node;

      fread( &node, sizeof( QBSPNode ), 1, file );

      nodes[i].plane = node.plane;
      nodes[i].front = node.front;
      nodes[i].back  = node.back;
    }

    nLeaves = int( lumps[QBSP_LUMP_LEAFS].length / sizeof( QBSPLeaf ) );
    leaves = new BSP::Leaf[nLeaves];
    fseek( file, lumps[QBSP_LUMP_LEAFS].offset, SEEK_SET );

    for( int i = 0; i < nLeaves; ++i ) {
      QBSPLeaf leaf;

      fread( &leaf, sizeof( QBSPLeaf ), 1, file );

      leaves[i].firstFace = leaf.firstFace;
      leaves[i].nFaces    = leaf.nFaces;
      leaves[i].cluster   = leaf.cluster;
    }

    nEntityModels = int( lumps[QBSP_LUMP_MODELS].length / sizeof( QBSPModel ) );
    entityModels = new EntityModel[nEntityModels];
    fseek( file, lumps[QBSP_LUMP_MODELS].offset, SEEK_SET );

    if( nEntityModels < 1 ) {
      log.println( "BSP should contain at least 1 model (entire BSP)" );
      return false;
    }

    for( int i = 0; i < nEntityModels; ++i ) {
      QBSPModel model;

      fread( &model, sizeof( QBSPModel ), 1, file );

      entityModels[i].firstFace = model.firstFace;
      entityModels[i].nFaces    = model.nFaces;
    }

    nVertices = int( lumps[QBSP_LUMP_VERTICES].length / sizeof( QBSPVertex ) );
    vertices = new BSP::Vertex[nVertices];
    fseek( file, lumps[QBSP_LUMP_VERTICES].offset, SEEK_SET );

    for( int i = 0; i < nVertices; ++i ) {
      QBSPVertex vertex;

      fread( &vertex, sizeof( QBSPVertex ), 1, file );

      vertices[i].p             = Point3::ORIGIN + ( Point3( vertex.p ) - Point3::ORIGIN ) * scale;
      vertices[i].texCoord      = TexCoord( vertex.texCoord[0], vertex.texCoord[1] );
      vertices[i].lightmapCoord = TexCoord( vertex.lightmapCoord[0], vertex.lightmapCoord[1] );
    }

    nIndices = int( lumps[QBSP_LUMP_INDICES].length / sizeof( int ) );
    indices = new int[nIndices];
    fseek( file, lumps[QBSP_LUMP_INDICES].offset, SEEK_SET );
    fread( indices, sizeof( int ), nIndices, file );

    nLeafFaces = int( lumps[QBSP_LUMP_LEAFFACES].length / sizeof( int ) );
    leafFaces = new int[nLeafFaces];
    fseek( file, lumps[QBSP_LUMP_LEAFFACES].offset, SEEK_SET );
    fread( leafFaces, sizeof( int ), nLeafFaces, file );

    nFaces = int( lumps[QBSP_LUMP_FACES].length / sizeof( QBSPFace ) );
    faces = new BSP::Face[nFaces];
    fseek( file, lumps[QBSP_LUMP_FACES].offset, SEEK_SET );

    for( int i = 0; i < nFaces; ++i ) {
      QBSPFace face;

      fread( &face, sizeof( QBSPFace ), 1, file );

      faces[i].texture     = face.texture;
      faces[i].lightmap    = face.lightmap;
      faces[i].material    = ( texTypes[face.texture] & QBSP_WATER_BIT ) ? Material::WATER_BIT : 0;
      faces[i].normal      = Vec3( face.normal );
      faces[i].firstVertex = face.firstVertex;
      faces[i].nVertices   = face.nVertices;
      faces[i].firstIndex  = face.firstIndex;
      faces[i].nIndices    = face.nIndices;
    }

    nLightmaps = int( lumps[QBSP_LUMP_LIGHTMAPS].length / sizeof( BSP::Lightmap ) );

    if( nLightmaps != 0 ) {
      lightmaps = new BSP::Lightmap[nLightmaps];

      fseek( file, lumps[QBSP_LUMP_LIGHTMAPS].offset, SEEK_SET );
      fread( lightmaps, sizeof( BSP::Lightmap ), nLightmaps, file );

      fseek( file, lumps[QBSP_LUMP_VISUALDATA].offset, SEEK_SET );
      fread( &visual.nClusters, sizeof( int ), 1, file );
      fread( &visual.clusterLength, sizeof( int ), 1, file );
    }
    else {
      lightmaps = null;
    }

//     if( lumps[QBSP_LUMP_VISUALDATA].length != 0 ) {
//       visual.bitsets = new Bitset[visual.nClusters];
//
//       for( int i = 0; i < visual.nClusters; ++i ) {
//         visual.bitsets[i].setSize( visual.clusterLength * 8 );
//         fread( visual.bitsets[i], sizeof( char ), visual.clusterLength, file );
//       }
//     }
//     else {
//       visual.bitsets = null;
//     }

    delete[] texFlags;
    delete[] texTypes;

    fclose( file );

    return true;
  }

  void BSP::freeQBSP( const char* name )
  {
    log.print( "Freeing Quake 3 BSP model '%s' ...", name );

    delete[] textures;
    delete[] planes;
    delete[] nodes;
    delete[] leaves;
    delete[] entityModels;
    delete[] vertices;
    delete[] indices;
    delete[] leafFaces;
    delete[] faces;
    delete[] lightmaps;

    textures     = null;
    planes       = null;
    nodes        = null;
    leaves       = null;
    vertices     = null;
    indices      = null;
    leafFaces    = null;
    faces        = null;
    lightmaps    = null;

    nTextures     = 0;
    nPlanes       = 0;
    nNodes        = 0;
    nLeaves       = 0;
    nEntityModels = 0;
    nVertices     = 0;
    nIndices      = 0;
    nLeafFaces    = 0;
    nFaces        = 0;
    nLightmaps    = 0;

    log.printEnd( " OK" );
  }

  bool BSP::save( const char* file )
  {
    log.print( "Dumping BSP model to '%s' ...", file );

    int size = 0;

    size += 1             * int( sizeof( Bounds ) );
    size += 10            * int( sizeof( int ) );
    size += nTextures     * int( 64 * sizeof( char ) );
    size += nPlanes       * int( sizeof( Plane ) );
    size += nNodes        * int( sizeof( Node ) );
    size += nLeaves       * int( sizeof( Leaf ) );
    size += nEntityModels * int( sizeof( EntityClass ) );
    size += nVertices     * int( sizeof( Vertex ) );
    size += nIndices      * int( sizeof( int ) );
    size += nLeafFaces    * int( sizeof( int ) );
    size += nFaces        * int( sizeof( Face ) );
    size += nLightmaps    * int( sizeof( Lightmap ) );

    Buffer buffer( size );
    OutputStream os = buffer.outputStream();

    os.writePoint3( mins );
    os.writePoint3( maxs );

    os.writeInt( nTextures );
    os.writeInt( nPlanes );
    os.writeInt( nNodes );
    os.writeInt( nLeaves );
    os.writeInt( nEntityModels );
    os.writeInt( nVertices );
    os.writeInt( nIndices );
    os.writeInt( nLeafFaces );
    os.writeInt( nFaces );
    os.writeInt( nLightmaps );

    for( int i = 0; i < nTextures; ++i ) {
      if( textures[i] == -1 ) {
        os.writePaddedString( "", 64 );
      }
      else {
        os.writePaddedString( translator.textures[ textures[i] ].name, 64 );
      }
    }

    for( int i = 0; i < nPlanes; ++i ) {
      os.writeVec3( planes[i].normal );
      os.writeFloat( planes[i].distance );
    }

    for( int i = 0; i < nNodes; ++i ) {
      os.writeInt( nodes[i].plane );
      os.writeInt( nodes[i].front );
      os.writeInt( nodes[i].back );
    }

    for( int i = 0; i < nLeaves; ++i ) {
      os.writeInt( leaves[i].firstFace );
      os.writeInt( leaves[i].nFaces );
      os.writeInt( leaves[i].cluster );
    }

    for( int i = 0; i < nEntityModels; ++i ) {
      os.writeInt( entityModels[i].firstFace );
      os.writeInt( entityModels[i].nFaces );
    }

    for( int i = 0; i < nVertices; ++i ) {
      os.writePoint3( vertices[i].p );
      os.writeFloat( vertices[i].texCoord.u );
      os.writeFloat( vertices[i].texCoord.v );
      os.writeFloat( vertices[i].lightmapCoord.u );
      os.writeFloat( vertices[i].lightmapCoord.v );
    }

    for( int i = 0; i < nIndices; ++i ) {
      os.writeInt( indices[i] );
    }

    for( int i = 0; i < nLeafFaces; ++i ) {
      os.writeInt( leafFaces[i] );
    }

    for( int i = 0; i < nFaces; ++i ) {
      os.writeVec3( faces[i].normal );
      os.writeInt( faces[i].texture );
      os.writeInt( faces[i].lightmap );
      os.writeInt( faces[i].material );
      os.writeInt( faces[i].firstVertex );
      os.writeInt( faces[i].nVertices );
      os.writeInt( faces[i].firstIndex );
      os.writeInt( faces[i].nIndices );
    }

    for( int i = 0; i < nLightmaps; ++i ) {
      os.writeChars( lightmaps[i].bits, LIGHTMAP_SIZE );
    }

    buffer.write( file );

    log.printEnd( " OK" );
    return true;
  }

  BSP::BSP() :
      nTextures( 0 ), nPlanes( 0 ), nNodes( 0 ), nLeaves( 0 ), nEntityModels( 0 ),
      nVertices( 0 ), nIndices( 0 ), nLeafFaces( 0 ), nFaces( 0 ), nLightmaps( 0 ),
      textures( null ), planes( null ), nodes( null ), leaves( null ), entityModels( null ),
      vertices( null ), indices( null ), leafFaces( null ), faces( null ), lightmaps( null ),
      texIds( null ), lightmapIds( null )
  {}

  void BSP::prebuild( const char* name )
  {
    log.println( "Prebuilding Quake 3 BSP model '%s' {", name );
    log.indent();

    BSP* bsp = new BSP();
    bsp->name = name;

    if( !bsp->loadQBSP( String( "maps/" ) + name ) ) {
      bsp->freeQBSP( name );
      log.unindent();
      log.println( "}" );
      throw Exception( "Client QBSP loading failed" );
    }

    bsp->save( String( "maps/" ) + name + String( ".ozcBSP" ) );
    bsp->freeQBSP( name );
    delete bsp;

    log.unindent();
    log.println( "}" );
  }

  BSP::BSP( int bspIndex ) :
      nTextures( 0 ), nPlanes( 0 ), nNodes( 0 ), nLeaves( 0 ), nEntityModels( 0 ),
      nVertices( 0 ), nIndices( 0 ), nLeafFaces( 0 ), nFaces( 0 ), nLightmaps( 0 ),
      textures( null ), planes( null ), nodes( null ), leaves( null ), entityModels( null ),
      vertices( null ), indices( null ), leafFaces( null ), faces( null ), lightmaps( null ),
      texIds( null ), lightmapIds( null ), isUpdated( false )
  {
    const String& name = translator.bsps[bspIndex].name;

    bsp = orbis.bsps[bspIndex];
    assert( bsp != null );

    log.println( "Loading BSP model '%s' {", name.cstr() );
    log.indent();

    if( !loadOZCBSP( "maps/" + name + ".ozcBSP" ) ) {
      log.println( "Failed" );
      freeOZCBSP();
      throw Exception( "Client ozcBSP loading failed" );
    }

    texIds = new uint[nTextures];
    for( int i = 0; i < nTextures; ++i ) {
      if( textures[i] != -1 ) {
        texIds[i] = context.requestTexture( textures[i] );
      }
    }

    if( nLightmaps != 0 ) {
      lightmapIds = new uint[nLightmaps];
      for( int i = 0; i < nLightmaps; ++i ) {
        lightmapIds[i] = context.createTexture( lightmaps[i].bits,
                                                LIGHTMAP_DIM, LIGHTMAP_DIM, LIGHTMAP_BPP );
      }
    }

    hiddenFaces.setSize( nFaces );
    drawnFaces.setSize( nFaces );
    hiddenFaces.clearAll();

    if( visual.bitsets != null ) {
      visibleLeafs.setSize( visual.bitsets[0].length() );
    }

    for( int i = 0; i < nFaces; ++i ) {
      const Vertex* verts = &vertices[ faces[i].firstVertex ];

      for( int j = 0; j < faces[i].nVertices; ++j ) {
        if( verts[j].p.x < mins.x || verts[j].p.x > maxs.x ||
            verts[j].p.y < mins.y || verts[j].p.y > maxs.y ||
            verts[j].p.z < mins.z || verts[j].p.z > maxs.z )
        {
          hiddenFaces.set( i );
          break;
        }
      }
    }

    log.unindent();
    log.println( "}" );
  }

  BSP::~BSP()
  {
    for( int i = 0; i < nTextures; ++i ) {
      if( textures[i] != -1 ) {
        context.releaseTexture( textures[i] );
      }
    }
    delete[] texIds;

    for( int i = 0; i < nLightmaps; ++i ) {
      context.freeTexture( lightmapIds[i] );
    }
    delete[] lightmapIds;

    freeOZCBSP();
  }

  int BSP::draw( const Struct* str_ )
  {
    str = str_;
    camPos = camera.p + ( Point3::ORIGIN - str->p );

    if( nLightmaps != 0 ) {
      glActiveTexture( GL_TEXTURE1 );
      glEnable( GL_TEXTURE_2D );
      glActiveTexture( GL_TEXTURE0 );
    }
    else {
      glActiveTexture( GL_TEXTURE1 );
      glDisable( GL_TEXTURE_2D );
      glActiveTexture( GL_TEXTURE0 );
    }

    glClientActiveTexture( GL_TEXTURE0 );

    glPushMatrix();
    glTranslatef( str->p.x, str->p.y, str->p.z );
    glRotatef( 90.0f * float( str->rot ), 0.0f, 0.0f, 1.0f );

    aCopy<ulong>( drawnFaces, hiddenFaces, hiddenFaces.unitLength() );

    int leaf = getLeaf();
    if( isInWater() ) {
      waterFlags |= IN_WATER_BRUSH;
    }

    if( visual.bitsets != null ) {
      int     cluster = leaves[leaf].cluster;
      Bitset& bitset  = visual.bitsets[cluster];

      for( int i = 0; i < nLeaves; ++i ) {
        const Leaf& leaf = leaves[i];
        Bounds rotatedLeaf = rotateBounds( leaf, str->rot );

        if( ( cluster == -1 || bitset.get( cluster ) ) &&
            frustum.isVisible( rotatedLeaf + ( str->p - Point3::ORIGIN ) ) )
        {
          for( int j = 0; j < leaf.nFaces; ++j ) {
            int faceIndex = leafFaces[leaf.firstFace + j];
            const Face& face = faces[faceIndex];

            if( !drawnFaces.get( faceIndex ) ) {
              drawFace( &face );
              drawnFaces.set( faceIndex );
            }
          }
        }
      }
    }
    else {
      for( int i = 0; i < nLeaves; ++i ) {
        const Leaf& leaf = leaves[i];
        Bounds rotatedLeaf = rotateBounds( leaf, str->rot );

        if( frustum.isVisible( rotatedLeaf + ( str->p - Point3::ORIGIN ) ) ) {
          for( int j = 0; j < leaf.nFaces; ++j ) {
            int faceIndex = leafFaces[leaf.firstFace + j];
            const Face& face = faces[faceIndex];

            if( !drawnFaces.get( faceIndex ) ) {
              drawFace( &face );
              drawnFaces.set( faceIndex );
            }
          }
        }
      }
    }
    glPopMatrix();

    return waterFlags;
  }

  void BSP::drawWater( const Struct* str_ )
  {
    str = str_;
    camPos = camera.p + ( Point3::ORIGIN - str->p );

    if( nLightmaps != 0 ) {
      glActiveTexture( GL_TEXTURE1 );
      glEnable( GL_TEXTURE_2D );
      glActiveTexture( GL_TEXTURE0 );
    }
    else {
      glActiveTexture( GL_TEXTURE1 );
      glDisable( GL_TEXTURE_2D );
      glActiveTexture( GL_TEXTURE0 );
    }
    glPushMatrix();
    glTranslatef( str->p.x, str->p.y, str->p.z );
    glRotatef( 90.0f * float( str->rot ), 0.0f, 0.0f, 1.0f );

    aCopy<ulong>( drawnFaces, hiddenFaces, hiddenFaces.unitLength() );

    int leaf = getLeaf();

    if( visual.bitsets != null ) {
      int     cluster = leaves[leaf].cluster;
      Bitset& bitset  = visual.bitsets[cluster];

      for( int i = 0; i < nLeaves; ++i ) {
        const Leaf& leaf = leaves[i];
        Bounds rotatedLeaf = rotateBounds( leaf, str->rot );

        if( ( cluster == -1 || bitset.get( cluster ) ) &&
            frustum.isVisible( rotatedLeaf + ( str->p - Point3::ORIGIN ) ) )
        {
          for( int j = 0; j < leaf.nFaces; ++j ) {
            int faceIndex = leafFaces[leaf.firstFace + j];
            const Face& face = faces[faceIndex];

            if( ( face.material & Material::WATER_BIT ) && !drawnFaces.get( faceIndex ) ) {
              drawFaceWater( &face );
              drawnFaces.set( faceIndex );
            }
          }
        }
      }
    }
    else {
      for( int i = 0; i < nLeaves; ++i ) {
        const Leaf& leaf = leaves[i];
        Bounds rotatedLeaf = rotateBounds( leaf, str->rot );

        if( frustum.isVisible( rotatedLeaf + ( str->p - Point3::ORIGIN ) ) ) {
          for( int j = 0; j < leaf.nFaces; ++j ) {
            int faceIndex = leafFaces[leaf.firstFace + j];
            const Face& face = faces[faceIndex];

            if( ( face.material & Material::WATER_BIT ) && !drawnFaces.get( faceIndex ) ) {
              drawFaceWater( &face );
              drawnFaces.set( faceIndex );
            }
          }
        }
      }
    }
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colours::WHITE );
    glPopMatrix();
  }

  int BSP::fullDraw( const Struct* str_ )
  {
    str = str_;
    camPos = camera.p + ( Point3::ORIGIN - str->p );

    if( nLightmaps != 0 ) {
      glActiveTexture( GL_TEXTURE1 );
      glEnable( GL_TEXTURE_2D );
      glActiveTexture( GL_TEXTURE0 );
    }
    else {
      glActiveTexture( GL_TEXTURE1 );
      glDisable( GL_TEXTURE_2D );
      glActiveTexture( GL_TEXTURE0 );
    }

    glClientActiveTexture( GL_TEXTURE0 );

    glPushMatrix();
    glTranslatef( str->p.x, str->p.y, str->p.z );
    glRotatef( 90.0f * float( str->rot ), 0.0f, 0.0f, 1.0f );

    if( isInWater() ) {
      waterFlags |= IN_WATER_BRUSH;
    }

    for( int i = 0; i < nEntityModels; ++i ) {
      const EntityModel& model = entityModels[i];
      const Vec3& entityPos = str->entities[i].offset;

      glPushMatrix();
      glTranslatef( entityPos.x, entityPos.y, entityPos.z );

      for( int j = 0; j < model.nFaces; ++j ) {
        const Face& face = faces[ model.firstFace + j ];

        if( !hiddenFaces.get( model.firstFace + j ) ) {
          drawFace( &face );
        }
      }
      glPopMatrix();
    }
    glPopMatrix();

    return waterFlags;
  }

  void BSP::fullDrawWater( const Struct* str_ )
  {
    str = str_;
    camPos = camera.p + ( Point3::ORIGIN - str->p );

    if( nLightmaps != 0 ) {
      glActiveTexture( GL_TEXTURE1 );
      glEnable( GL_TEXTURE_2D );
      glActiveTexture( GL_TEXTURE0 );
    }
    else {
      glActiveTexture( GL_TEXTURE1 );
      glDisable( GL_TEXTURE_2D );
      glActiveTexture( GL_TEXTURE0 );
    }
    glPushMatrix();
    glTranslatef( str->p.x, str->p.y, str->p.z );
    glRotatef( 90.0f * float( str->rot ), 0.0f, 0.0f, 1.0f );

    for( int i = 0; i < nFaces; ++i ) {
      if( ( faces[i].material & Material::WATER_BIT ) && !hiddenFaces.get( i ) ) {
        drawFaceWater( &faces[i] );
      }
    }
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colours::WHITE );
    glPopMatrix();
  }

  uint BSP::genList()
  {
    uint list = context.genList();

    glNewList( list, GL_COMPILE );

    aCopy<ulong>( drawnFaces, hiddenFaces, hiddenFaces.unitLength() );

    for( int i = 0; i < nLeaves; ++i ) {
      const Leaf& leaf = leaves[i];

      for( int j = 0; j < leaf.nFaces; ++j ) {
        int face = leafFaces[leaf.firstFace + j];

        if( !drawnFaces.get( face ) ) {
          drawFace( &faces[face] );
          drawnFaces.set( face );
        }
      }
    }
    glEndList();

    return list;
  }

  void BSP::beginRender()
  {
    waterFlags = 0;

    glFrontFace( GL_CW );

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
  }

  void BSP::endRender()
  {
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

    glActiveTexture( GL_TEXTURE1 );
    glDisable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE0 );

    glFrontFace( GL_CCW );
  }

}
}
