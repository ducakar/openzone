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

  Point3 BSP::camPos;
  int    BSP::waterFlags;

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

  bool BSP::loadOZCBSP( const char* fileName )
  {
    Buffer buffer;
    buffer.read( fileName );

    if( buffer.isEmpty() ) {
      return false;
    }

    InputStream is = buffer.inputStream();

    nTextures     = is.readInt();
    nEntityModels = is.readInt();
    nVertices     = is.readInt();
    nIndices      = is.readInt();
    nFaces        = is.readInt();
    nLightmaps    = is.readInt();

    int size = 0;

    size += nTextures     * int( 64 * sizeof( char ) );
    size = Alloc::alignUp( size );
    size += nEntityModels * int( sizeof( EntityModel ) );
    size = Alloc::alignUp( size );
    size += nVertices     * int( sizeof( Vertex ) );
    size += nIndices      * int( sizeof( int ) );
    size = Alloc::alignUp( size );
    size += nFaces        * int( sizeof( Face ) );
    size += nLightmaps    * int( sizeof( Lightmap ) );

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
      vertices[i].normal = is.readVec3();
      vertices[i].texCoord.u = is.readFloat();
      vertices[i].texCoord.v = is.readFloat();
      vertices[i].lightmapCoord.u = is.readFloat();
      vertices[i].lightmapCoord.v = is.readFloat();
    }
    data += nVertices * sizeof( Vertex );

    indices = new( data ) int[nIndices];
    for( int i = 0; i < nIndices; ++i ) {
      indices[i] = is.readInt();
    }
    data += nIndices * sizeof( int );

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

    assert( !is.isAvailable() );

    return true;
  }

  void BSP::freeOZCBSP()
  {
    log.print( "Freeing BSP model '%s' ...", name.cstr() );

    if( textures != null ) {
      aDestruct( textures, nTextures );
      aDestruct( entityModels, nEntityModels );
      aDestruct( vertices, nVertices );
      aDestruct( indices, nIndices );
      aDestruct( faces, nFaces );
      aDestruct( lightmaps, nLightmaps );

      Alloc::dealloc( textures );

      nTextures     = 0;
      nEntityModels = 0;
      nVertices     = 0;
      nIndices      = 0;
      nFaces        = 0;
      nLightmaps    = 0;

      textures     = null;
      entityModels = null;
      vertices     = null;
      indices      = null;
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

      // disable if out of bounds
      const Vertex* verts = &vertices[ faces[i].firstVertex ];
      for( int j = 0; j < faces[i].nVertices; ++j ) {
        if( verts[j].p.x < -maxDim || verts[j].p.x > maxDim ||
            verts[j].p.y < -maxDim || verts[j].p.y > maxDim ||
            verts[j].p.z < -maxDim || verts[j].p.z > maxDim )
        {
          faces[i].nIndices = 0;
          break;
        }
      }
    }

    nLightmaps = int( lumps[QBSP_LUMP_LIGHTMAPS].length / sizeof( BSP::Lightmap ) );

    if( nLightmaps != 0 ) {
      lightmaps = new BSP::Lightmap[nLightmaps];

      fseek( file, lumps[QBSP_LUMP_LIGHTMAPS].offset, SEEK_SET );
      fread( lightmaps, sizeof( BSP::Lightmap ), nLightmaps, file );
    }
    else {
      lightmaps = null;
    }

    delete[] texFlags;
    delete[] texTypes;

    fclose( file );

    return true;
  }

  void BSP::freeQBSP( const char* name )
  {
    log.print( "Freeing Quake 3 BSP model '%s' ...", name );

    delete[] textures;
    delete[] entityModels;
    delete[] vertices;
    delete[] indices;
    delete[] faces;
    delete[] lightmaps;

    textures  = null;
    vertices  = null;
    indices   = null;
    faces     = null;
    lightmaps = null;

    nTextures     = 0;
    nEntityModels = 0;
    nVertices     = 0;
    nIndices      = 0;
    nFaces        = 0;
    nLightmaps    = 0;

    log.printEnd( " OK" );
  }

  void BSP::optimise()
  {/*
    log.println( "Optimising BSP model {" );
    log.indent();

    // remove faces that lay out of boundaries
    for( int i = 0; i < nFaces; ) {
      assert( faces[i].nVertices > 0 && faces[i].nIndices >= 0 );

      if( faces[i].nIndices != 0 ) {
        ++i;
        continue;
      }

      aRemove( faces, i, nFaces );
      --nFaces;
      log.print( "outside face removed " );

      // adjust face references (for leaves)
      for( int j = 0; j < nLeafFaces; ) {
        if( leafFaces[j] < i ) {
          ++j;
        }
        else if( i < leafFaces[j] ) {
          --leafFaces[j];
          ++j;
        }
        else {
          aRemove( leafFaces, j, nLeafFaces );
          --nLeafFaces;
          log.printRaw( "." );

          for( int k = 0; k < nLeaves; ++k ) {
            if( j < leaves[k].firstFace ) {
              --leaves[k].firstFace;
            }
            else if( j < leaves[k].firstFace + leaves[k].nFaces ) {
              assert( leaves[k].nFaces > 0 );

              --leaves[k].nFaces;
            }
          }
        }
      }
      // adjust face references (for models)
      for( int j = 0; j < nEntityModels; ++j ) {
        if( i < entityModels[j].firstFace ) {
          --entityModels[j].firstFace;
        }
        else if( i < entityModels[j].firstFace + entityModels[j].nFaces ) {
          assert( entityModels[j].nFaces > 0 );

          --entityModels[j].nFaces;
        }
      }
      log.printEnd();
    }

    faces = aRealloc( faces, nFaces, nFaces );
    leafFaces = aRealloc( leafFaces, nLeafFaces, nLeafFaces);

    // remove unreferenced and empty leaves
    log.print( "removing unreferenced and empty leaves " );

    for( int i = 0; i < nLeaves; ) {
      bool isReferenced = false;

      for( int j = 0; j < nNodes; ++j ) {
        if( nodes[j].front == ~i || nodes[j].back == ~i ) {
          isReferenced = true;
          break;
        }
      }

      if( isReferenced && leaves[i].nFaces != 0 ) {
        ++i;
        continue;
      }

      aRemove( leaves, i, nLeaves );
      --nLeaves;
      log.printRaw( "." );

      // update references and tag unnecessary nodes, will be removed in the next pass (index 0 is
      // invalid as the root cannot be referenced)
      for( int j = 0; j < nNodes; ++j ) {
        if( ~nodes[j].front == i ) {
          nodes[j].front = 0;
        }
        else if( ~nodes[j].front > i ) {
          ++nodes[j].front;
        }

        if( ~nodes[j].back == i ) {
          nodes[j].back = 0;
        }
        else if( ~nodes[j].back > i ) {
          ++nodes[j].back;
        }
      }
    }

    leaves = aRealloc( leaves, nLeaves, nLeaves );

    log.printEnd( " OK" );

    // collapse unnecessary nodes
    log.print( "collapsing nodes " );

    bool hasCollapsed;
    do {
      hasCollapsed = false;

      for( int i = 0; i < nNodes; ++i ) {
        if( nodes[i].front == 0 ) {
          hasCollapsed = true;

          // find parent and bind the remaining leaf to the parent
          int j;
          for( j = 0; j < nNodes; ++j ) {
            if( nodes[j].front == i ) {
              nodes[j].front = nodes[i].back;
              break;
            }
            else if( nodes[j].back == i ) {
              nodes[j].back = nodes[i].back;
              break;
            }
          }
          assert( j < nNodes );

          log.printRaw( "." );
        }
        else if( nodes[i].back == 0 ) {
          hasCollapsed = true;

          // find parent and bind the remaining leaf to the parent
          int j;
          for( j = 0; j < nNodes; ++j ) {
            if( nodes[j].front == i ) {
              nodes[j].front = nodes[i].front;
              break;
            }
            else if( nodes[j].back == i ) {
              nodes[j].back = nodes[i].front;
              break;
            }
          }
          assert( j < nNodes );

          log.printRaw( "." );
        }

        // remove node and adjust references
        if( nodes[i].front == 0 || nodes[i].back == 0 ) {
          aRemove( nodes, i, nNodes );
          --nNodes;

          for( int j = 0; j < nNodes; ++j ) {
            assert( nodes[j].front != i && nodes[j].back != i );

            if( nodes[j].front > i && nodes[j].front != 0 ) {
              --nodes[j].front;
            }
            if( nodes[j].back > i && nodes[j].back != 0 ) {
              --nodes[j].back;
            }
          }
        }
      }
    }
    while( hasCollapsed );

    nodes = aRealloc( nodes, nNodes, nNodes );

    log.printEnd( " OK" );

    // integrity check
    Bitset usedNodes( nNodes );
    Bitset usedLeaves( nLeaves );

    usedNodes.clearAll();
    usedLeaves.clearAll();

    for( int i = 0; i < nNodes; ++i ) {
      if( nodes[i].front < 0 ) {
        usedLeaves.set( ~nodes[i].front );
      }
      else if( nodes[i].front != 0 ) {
        usedNodes.set( nodes[i].front );
      }

      if( nodes[i].back < 0 ) {
        usedLeaves.set( ~nodes[i].back );
      }
      else if( nodes[i].back != 0 ) {
        usedNodes.set( nodes[i].back );
      }
    }

    for( int i = 0; i < nLeaves; ++i ) {
      assert( usedLeaves.get( i ) );
    }
    for( int i = 1; i < nNodes; ++i ) {
      assert( usedNodes.get( i ) );
    }

    // remove unused indices
    log.print( "removing unused indices " );

    bool* usedIndices = new bool[nIndices];
    aSet( usedIndices, false, nIndices );

    for( int i = 0; i < nFaces; ++i ) {
      for( int j = 0; j < faces[i].nIndices; ++j ) {
        usedIndices[ faces[i].firstIndex + j ] = true;
      }
    }

    for( int i = 0; i < nIndices; ) {
      if( usedIndices[i] ) {
        ++i;
        continue;
      }

      aRemove( indices, i, nIndices );
      aRemove( usedIndices, i, nIndices );
      --nIndices;
      log.printRaw( "." );

      for( int j = 0; j < nFaces; ++j ) {
        if( i < faces[j].firstIndex ) {
          --faces[j].firstIndex;
        }
        else if( i < faces[j].firstIndex + faces[j].nIndices ) {
          // removed index shouldn't be referenced by any face
          assert( false );
        }
      }
    }

    log.printEnd( " OK" );

    delete[] usedIndices;
    indices = aRealloc( indices, nIndices, nIndices );

    // remove unused vertices
    log.print( "removing unused vertices " );

    bool* usedVertices = new bool[nVertices];
    aSet( usedVertices, false, nVertices );

    for( int i = 0; i < nIndices; ++i ) {
      usedVertices[ indices[i] ] = true;
    }

    for( int i = 0; i < nVertices; ) {
      if( usedVertices[i] ) {
        ++i;
        continue;
      }

      aRemove( vertices, i, nVertices );
      aRemove( usedVertices, i, nVertices );
      --nVertices;
      log.printRaw( "." );

      for( int j = 0; j < nIndices; ++j ) {
        if( i < indices[j] ) {
          --indices[j];
        }
        else if( i == indices[j] ) {
          // removed vertex shouldn't be referenced by any index
          assert( false );
        }
      }
    }

    log.printEnd( " OK" );

    delete[] usedVertices;
    vertices = aRealloc( vertices, nVertices, nVertices );

    // remove unused planes
    log.print( "removing unused planes " );

    bool* usedPlanes = new bool[nPlanes];
    aSet( usedPlanes, false, nPlanes );

    for( int i = 0; i < nNodes; ++i ) {
      usedPlanes[ nodes[i].plane ] = true;
    }

    for( int i = 0; i < nPlanes; ) {
      if( usedPlanes[i] ) {
        ++i;
        continue;
      }

      aRemove( planes, i, nPlanes );
      aRemove( usedPlanes, i, nPlanes );
      --nPlanes;
      log.printRaw( "." );

      // adjust plane references
      for( int j = 0; j < nNodes; ++j ) {
        assert( nodes[j].plane != i );

        if( nodes[j].plane > i ) {
          --nodes[j].plane;
        }
      }
    }

    delete[] usedPlanes;
    planes = aRealloc( planes, nPlanes, nPlanes );

    log.printEnd( " OK" );

    log.unindent();
    log.println( "}" );*/
  }

  bool BSP::save( const char* file )
  {
    log.print( "Dumping BSP model to '%s' ...", file );

    int size = 0;

    size += 6             * int( sizeof( int ) );
    size += nTextures     * int( 64 * sizeof( char ) );
    size += nEntityModels * int( sizeof( EntityModel ) );
    size += nVertices     * int( sizeof( Vertex ) );
    size += nIndices      * int( sizeof( int ) );
    size += nFaces        * int( sizeof( Face ) );
    size += nLightmaps    * int( LIGHTMAP_SIZE * sizeof( char ) );

    Buffer buffer( size );
    OutputStream os = buffer.outputStream();

    os.writeInt( nTextures );
    os.writeInt( nEntityModels );
    os.writeInt( nVertices );
    os.writeInt( nIndices );
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

    for( int i = 0; i < nEntityModels; ++i ) {
      os.writeInt( entityModels[i].firstFace );
      os.writeInt( entityModels[i].nFaces );
    }

    for( int i = 0; i < nVertices; ++i ) {
      os.writePoint3( vertices[i].p );
      os.writeVec3( vertices[i].normal );
      os.writeFloat( vertices[i].texCoord.u );
      os.writeFloat( vertices[i].texCoord.v );
      os.writeFloat( vertices[i].lightmapCoord.u );
      os.writeFloat( vertices[i].lightmapCoord.v );
    }

    for( int i = 0; i < nIndices; ++i ) {
      os.writeInt( indices[i] );
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

    assert( !os.isAvailable() );
    buffer.write( file );

    log.printEnd( " OK" );
    return true;
  }

  BSP::BSP() :
      nTextures( 0 ), nEntityModels( 0 ), nVertices( 0 ), nIndices( 0 ), nFaces( 0 ),
      nLightmaps( 0 ),
      textures( null ), entityModels( null ), vertices( null ), indices( null ), faces( null ),
      lightmaps( null ),
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

    bsp->optimise();
    bsp->save( String( "maps/" ) + name + String( ".ozcBSP" ) );
    bsp->freeQBSP( name );
    delete bsp;

    log.unindent();
    log.println( "}" );
  }

  BSP::BSP( int bspIndex ) :
      nTextures( 0 ), nEntityModels( 0 ), nVertices( 0 ), nIndices( 0 ), nFaces( 0 ),
      nLightmaps( 0 ),
      textures( null ), entityModels( null ), vertices( null ), indices( null ), faces( null ),
      lightmaps( null ),
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

  int BSP::fullDraw( const Struct* str )
  {
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

  void BSP::fullDrawWater( const Struct* str )
  {
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
