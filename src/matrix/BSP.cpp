/*
 *  BSP.cpp
 *
 *  Data structure for Quake3 BSP level
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/BSP.hpp"

#include "matrix/Timer.hpp"
#include "matrix/Translator.hpp"

namespace oz
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
    Vec3  normal;
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

  struct QBSPBrush
  {
    int firstSide;
    int nSides;
    int texture;
  };

  struct QBSPBrushSide
  {
    int plane;
    int texture;
  };

  struct QBSPVertex
  {
    Vec3  p;
    float texCoord[2];
    float lightmapCoord[2];
    Vec3  normal;
    char  colour[4];
  };

  struct QBSPFace
  {
    int  texture;
    int  effect;
    int  type;

    int  firstVertex;
    int  nVertices;

    int  firstIndex;
    int  nIndices;

    int  lightmap;
    int  lightmapCorner[2];
    int  lightmapSize[2];

    Vec3 lightmapPos;
    Vec3 lightmapVecs[2];

    Vec3 normal;

    int  size[2];
  };

#ifndef OZ_USE_PREBUILT

  inline bool BSP::includes( const BSP::Brush& brush ) const
  {
    for( int i = 0; i < brush.nSides; ++i ) {
      const Plane& plane = planes[ brushSides[brush.firstSide + i] ];

      float offset =
          Math::abs( plane.normal.x * maxDim ) +
          Math::abs( plane.normal.y * maxDim ) +
          Math::abs( plane.normal.z * maxDim );

      if( offset <= plane.distance ) {
        return false;
      }
    }
    return true;
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
    maxDim = bspConfig.get( "maxDim", Math::inf() );
    life = bspConfig.get( "life", 1000.0f );

    if( Math::isNaN( scale ) || Math::isNaN( maxDim ) ) {
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

      planes[i].normal   = plane.normal;
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

    mins.x = Math::inf();
    mins.y = Math::inf();
    mins.z = Math::inf();

    maxs.x = -Math::inf();
    maxs.y = -Math::inf();
    maxs.z = -Math::inf();

    nLeaves = int( lumps[QBSP_LUMP_LEAFS].length / sizeof( QBSPLeaf ) );
    leaves = new BSP::Leaf[nLeaves];
    fseek( file, lumps[QBSP_LUMP_LEAFS].offset, SEEK_SET );

    for( int i = 0; i < nLeaves; ++i ) {
      QBSPLeaf leaf;

      fread( &leaf, sizeof( QBSPLeaf ), 1, file );

      leaves[i].mins.x = float( leaf.bb[0][0] ) * scale;
      leaves[i].mins.y = float( leaf.bb[0][1] ) * scale;
      leaves[i].mins.z = float( leaf.bb[0][2] ) * scale;

      leaves[i].maxs.x = float( leaf.bb[1][0] ) * scale;
      leaves[i].maxs.y = float( leaf.bb[1][1] ) * scale;
      leaves[i].maxs.z = float( leaf.bb[1][2] ) * scale;

      leaves[i].firstBrush = leaf.firstBrush;
      leaves[i].nBrushes   = leaf.nBrushes;
      leaves[i].firstFace  = leaf.firstFace;
      leaves[i].nFaces     = leaf.nFaces;
      leaves[i].cluster    = leaf.cluster;

      if( -maxDim <= leaves[i].mins.x && leaves[i].maxs.x <= maxDim &&
          -maxDim <= leaves[i].mins.y && leaves[i].maxs.y <= maxDim &&
          -maxDim <= leaves[i].mins.z && leaves[i].maxs.z <= maxDim )
      {
        mins.x = Math::min( mins.x, leaves[i].mins.x );
        mins.y = Math::min( mins.y, leaves[i].mins.y );
        mins.z = Math::min( mins.z, leaves[i].mins.z );

        maxs.x = Math::max( maxs.x, leaves[i].maxs.x );
        maxs.y = Math::max( maxs.y, leaves[i].maxs.y );
        maxs.z = Math::max( maxs.z, leaves[i].maxs.z );
      }
    }

    nLeafFaces = int( lumps[QBSP_LUMP_LEAFFACES].length / sizeof( int ) );
    leafFaces = new int[nLeafFaces];
    fseek( file, lumps[QBSP_LUMP_LEAFFACES].offset, SEEK_SET );
    fread( leafFaces, sizeof( int ), nLeafFaces, file );

    nLeafBrushes = int( lumps[QBSP_LUMP_LEAFBRUSHES].length / sizeof( int ) );
    leafBrushes = new int[nLeafBrushes];
    fseek( file, lumps[QBSP_LUMP_LEAFBRUSHES].offset, SEEK_SET );
    fread( leafBrushes, sizeof( int ), nLeafBrushes, file );

    nModels = int( lumps[QBSP_LUMP_MODELS].length / sizeof( QBSPModel ) );
    models = new BSP::Model[nModels];
    fseek( file, lumps[QBSP_LUMP_MODELS].offset, SEEK_SET );

    if( nModels < 1 ) {
      log.println( "BSP should contain at least 1 model (entire BSP)" );
      return false;
    }

    assert( nModels <= 99 );
    char keyBuffer[] = "model  ";

    for( int i = 0; i < nModels; ++i ) {
      QBSPModel model;

      fread( &model, sizeof( QBSPModel ), 1, file );

      models[i].mins.x = model.bb[0][0] * scale;
      models[i].mins.y = model.bb[0][1] * scale;
      models[i].mins.z = model.bb[0][2] * scale;

      models[i].maxs.x = model.bb[1][0] * scale;
      models[i].maxs.y = model.bb[1][1] * scale;
      models[i].maxs.z = model.bb[1][2] * scale;

      models[i].mins -= Vec3( 2.0f * EPSILON, 2.0f * EPSILON, 2.0f * EPSILON );
      models[i].maxs += Vec3( 2.0f * EPSILON, 2.0f * EPSILON, 2.0f * EPSILON );

      models[i].firstBrush = model.firstBrush;
      models[i].nBrushes   = model.nBrushes;
      models[i].firstFace  = model.firstFace;
      models[i].nFaces     = model.nFaces;

      keyBuffer[5] = char( '0' + i / 10 );
      keyBuffer[6] = char( '0' + i % 10 );
      String keyName = keyBuffer;

      models[i].move.x = bspConfig.get( keyName + ".move.x", 0.0f );
      models[i].move.y = bspConfig.get( keyName + ".move.y", 0.0f );
      models[i].move.z = bspConfig.get( keyName + ".move.z", 0.0f );

      models[i].ratioInc = bspConfig.get( keyName + ".ratioInc", Timer::TICK_TIME );
      models[i].flags = 0;

      String type = bspConfig.get( keyName + ".type", "BLOCKING" );
      if( type.equals( "IGNORING" ) ) {
        models[i].type = Model::IGNORING;
      }
      else if( type.equals( "BLOCKING" ) ) {
        models[i].type = Model::BLOCKING;
      }
      else if( type.equals( "PUSHING" ) ) {
        models[i].type = Model::PUSHING;
      }
      else if( type.equals( "CRUSHING" ) ) {
        models[i].type = Model::CRUSHING;
      }
      else {
        log.println( "invalid BSP model type, should be either IGNORING, BLOCKING, PUSHING or "
            "CRUSHING" );
        delete[] texFlags;
        delete[] texTypes;
        return false;
      }

      models[i].margin = bspConfig.get( keyName + ".margin", 1.0f );
      models[i].slideTime = 1.0f;
      models[i].timeout = 5.0f;
    }

    models[0].mins = mins;
    models[0].maxs = maxs;

    nBrushSides = int( lumps[QBSP_LUMP_BRUSHSIDES].length / sizeof( QBSPBrushSide ) );
    brushSides = new int[nBrushSides];
    fseek( file, lumps[QBSP_LUMP_BRUSHSIDES].offset, SEEK_SET );

    for( int i = 0; i < nBrushSides; ++i ) {
      QBSPBrushSide brushSide;

      fread( &brushSide, sizeof( QBSPBrushSide ), 1, file );

      brushSides[i] = brushSide.plane;
    }

    nBrushes = int( lumps[QBSP_LUMP_BRUSHES].length / sizeof( QBSPBrush ) );
    brushes = new BSP::Brush[nBrushes];
    fseek( file, lumps[QBSP_LUMP_BRUSHES].offset, SEEK_SET );

    for( int i = 0; i < nBrushes; ++i ) {
      QBSPBrush brush;

      fread( &brush, sizeof( QBSPBrush ), 1, file );

      brushes[i].firstSide = brush.firstSide;
      brushes[i].nSides    = brush.nSides;
      brushes[i].material  = 0;

      const int& flags = texFlags[brush.texture];
      const int& type  = texTypes[brush.texture];

      if( flags & QBSP_LADDER_BIT ) {
        brushes[i].material |= Material::LADDER_BIT;
      }
      if( !( flags & QBSP_NONSOLID_BIT ) ) {
        brushes[i].material |= Material::STRUCT_BIT;
      }
      if( flags & QBSP_SLICK_BIT ) {
        brushes[i].material |= Material::SLICK_BIT;
      }
      if( type & QBSP_WATER_BIT ) {
        brushes[i].material |= Material::WATER_BIT;
      }

      // brush out of bounds, mark it for exclusion
      if( !includes( brushes[i] ) ) {
        brushes[i].nSides = 0;
      }
    }

    if( nBrushes > MAX_BRUSHES ) {
      log.println( "Too many brushes %d, maximum is %d", nBrushes, MAX_BRUSHES );
      delete[] texFlags;
      delete[] texTypes;
      return false;
    }

    nVertices = int( lumps[QBSP_LUMP_VERTICES].length / sizeof( QBSPVertex ) );
    vertices = new BSP::Vertex[nVertices];
    fseek( file, lumps[QBSP_LUMP_VERTICES].offset, SEEK_SET );

    for( int i = 0; i < nVertices; ++i ) {
      QBSPVertex vertex;

      fread( &vertex, sizeof( QBSPVertex ), 1, file );

      vertices[i].p                = vertex.p * scale;
      vertices[i].texCoord[0]      = vertex.texCoord[0];
      vertices[i].texCoord[1]      = vertex.texCoord[1];
      vertices[i].lightmapCoord[0] = vertex.lightmapCoord[0];
      vertices[i].lightmapCoord[1] = vertex.lightmapCoord[1];
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
      faces[i].normal      = face.normal;
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

    if( lumps[QBSP_LUMP_VISUALDATA].length != 0 ) {
      visual.bitsets = new Bitset[visual.nClusters];
      for( int i = 0; i < visual.nClusters; ++i ) {
        visual.bitsets[i].setSize( visual.clusterLength * 8 );
        fread( visual.bitsets[i], sizeof( char ), visual.clusterLength, file );
      }
    }
    else {
      visual.bitsets = null;
    }

    delete[] texFlags;
    delete[] texTypes;

    fclose( file );

    return true;
  }

  void BSP::optimise()
  {
    // optimise
    log.println( "Optimising BSP {" );
    log.indent();

    // remove brushes that lay out of boundaries
    for( int i = 0; i < nBrushes; ) {
      if( brushes[i].nSides != 0 ) {
        ++i;
        continue;
      }

      aRemove( brushes, i, nBrushes );
      --nBrushes;
      log.print( "outside brush removed " );

      // adjust brush references (for leaves)
      for( int j = 0; j < nLeafBrushes; ) {
        if( leafBrushes[j] < i ) {
          ++j;
        }
        else if( i < leafBrushes[j] ) {
          --leafBrushes[j];
          ++j;
        }
        else {
          aRemove( leafBrushes, j, nLeafBrushes );
          --nLeafBrushes;
          log.printRaw( "." );

          for( int k = 0; k < nLeaves; ++k ) {
            if( j < leaves[k].firstBrush ) {
              --leaves[k].firstBrush;
            }
            else if( j < leaves[k].firstBrush + leaves[k].nBrushes ) {
              assert( leaves[k].nBrushes > 0 );

              --leaves[k].nBrushes;
            }
          }
        }
      }
      // adjust brush references (for models)
      for( int j = 0; j < nModels; ++j ) {
        if( i < models[j].firstBrush ) {
          --models[j].firstBrush;
        }
        else if( i < models[j].firstBrush + models[j].nBrushes ) {
          assert( models[j].nBrushes > 0 );

          --models[j].nBrushes;
        }
      }
      log.printEnd();
    }

    brushes = aRealloc( brushes, nBrushes, nBrushes );
    brushSides = aRealloc( brushSides, nBrushSides, nBrushSides );

    // remove unreferenced leaves
    log.print( "removing unreferenced leaves " );

    for( int i = 0; i < nLeaves; ) {
      bool isReferenced = false;

      for( int j = 0; j < nNodes; ++j ) {
        if( nodes[j].front == ~i || nodes[j].back == ~i ) {
          isReferenced = true;
        }
      }

      if( isReferenced && ( leaves[i].nBrushes != 0 || leaves[i].nFaces != 0 ) ) {
        ++i;
        continue;
      }

      aRemove( leaves, i, nLeaves );
      --nLeaves;
      log.printRaw( "." );

      // update references and tag unnecessary nodes, will be removed in the next pass (index 0 is
      // invalid as the root cannot be referenced)
      for( int j = 0; j < nNodes; ++j ) {
        if( nodes[j].front == ~i ) {
          nodes[j].front = 0;
        }
        else if( nodes[j].front < ~i ) {
          ++nodes[j].front;
        }

        if( nodes[j].back == ~i ) {
          nodes[j].back = 0;
        }
        else if( nodes[j].back < ~i ) {
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

        if( nodes[i].back == 0 ) {
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

    // remove brush sides and planes
    log.print( "removing brush sides " );

    bool* usedBrushSides = new bool[nBrushSides];
    bool* usedPlanes = new bool[nPlanes];

    aSet( usedBrushSides, false, nBrushSides );
    aSet( usedPlanes, false, nPlanes );

    for( int i = 0; i < nBrushes; ++i ) {
      for( int j = 0; j < brushes[i].nSides; ++j ) {
        usedBrushSides[ brushes[i].firstSide + j ] = true;
      }
    }

    for( int i = 0; i < nBrushSides; ) {
      if( usedBrushSides[i] ) {
        ++i;
        continue;
      }

      aRemove( brushSides, i, nBrushSides );
      aRemove( usedBrushSides, i, nBrushSides );
      --nBrushSides;
      log.printRaw( "." );

      for( int j = 0; j < nBrushes; ++j ) {
        if( i < brushes[j].firstSide ) {
          --brushes[j].firstSide;
        }
        else if( i < brushes[j].firstSide + brushes[j].nSides ) {
          // removed brush side shouldn't be referenced by any brush
          assert( false );
        }
      }
    }

    log.printEnd( " OK" );
    log.print( "removing planes " );

    for( int i = 0; i < nNodes; ++i ) {
      usedPlanes[ nodes[i].plane ] = true;
    }
    for( int i = 0; i < nBrushSides; ++i ) {
      usedPlanes[ brushSides[i] ] = true;
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
      for( int j = 0; j < nBrushSides; ++j ) {
        assert( brushSides[j] != i );

        if( brushSides[j] > i ) {
          --brushSides[j];
        }
      }
    }

    delete[] usedBrushSides;
    delete[] usedPlanes;

    brushSides = aRealloc( brushSides, nBrushSides, nBrushSides );
    planes = aRealloc( planes, nPlanes, nPlanes );

    log.printEnd( " OK" );

    // optimise bounds
    log.print( "Fitting bounds: " );

    mins = Vec3( +Math::inf(), +Math::inf(), +Math::inf() );
    maxs = Vec3( -Math::inf(), -Math::inf(), -Math::inf() );

    for( int i = 0; i < nBrushSides; ++i ) {
      Plane& plane = planes[ brushSides[i] ];

      if( plane.normal.x == -1.0f ) {
        mins.x = min( -plane.distance, mins.x );
      }
      else if( plane.normal.x == 1.0f ) {
        maxs.x = max( +plane.distance, maxs.x );
      }
      else if( plane.normal.y == -1.0f ) {
        mins.y = min( -plane.distance, mins.y );
      }
      else if( plane.normal.y == 1.0f ) {
        maxs.y = max( +plane.distance, maxs.y );
      }
      else if( plane.normal.z == -1.0f ) {
        mins.z = min( -plane.distance, mins.z );
      }
      else if( plane.normal.z == 1.0f ) {
        maxs.z = max( +plane.distance, maxs.z );
      }
    }

    models[0].mins = mins;
    models[0].maxs = maxs;

    log.printEnd( "(%g %g %g) (%g %g %g)", mins.x, mins.y, mins.z, maxs.x, maxs.y, maxs.z );

    log.unindent();
    log.println( "}" );
  }

  bool BSP::save( const char* fileName )
  {
    log.print( "Dumping BSP structure to '%s' ...", fileName );

    int size = 0;

    size += 1            * int( sizeof( Bounds ) );
    size += 2            * int( sizeof( float ) );
    size += 14           * int( sizeof( int ) );
    size += nPlanes      * int( sizeof( Plane ) );
    size += nNodes       * int( sizeof( Node ) );
    size += nLeaves      * int( sizeof( Leaf ) );
    size += nLeafFaces   * int( sizeof( int ) );
    size += nLeafBrushes * int( sizeof( int ) );
    size += nBrushes     * int( sizeof( Brush ) );
    size += nBrushSides  * int( sizeof( int ) );
    size += nModels      * int( sizeof( Model ) );
    size += nTextures    * int( 64 * sizeof( char ) );
    size += nVertices    * int( sizeof( Vertex ) );
    size += nIndices     * int( sizeof( int ) );
    size += nFaces       * int( sizeof( Face ) );
    size += nLightmaps   * int( sizeof( Lightmap ) );

    Buffer buffer( size );
    OutputStream os = buffer.outputStream();

    os.writeVec3( mins );
    os.writeVec3( maxs );
    os.writeFloat( maxDim );
    os.writeFloat( life );

    os.writeInt( nPlanes );
    os.writeInt( nNodes );
    os.writeInt( nLeaves );
    os.writeInt( nLeafFaces );
    os.writeInt( nLeafBrushes );
    os.writeInt( nBrushes );
    os.writeInt( nBrushSides );
    os.writeInt( nModels );
    os.writeInt( nTextures );
    os.writeInt( nVertices );
    os.writeInt( nIndices );
    os.writeInt( nFaces );
    os.writeInt( nLightmaps );

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
      os.writeVec3( leaves[i].mins );
      os.writeVec3( leaves[i].maxs );
      os.writeInt( leaves[i].firstBrush );
      os.writeInt( leaves[i].nBrushes );
      os.writeInt( leaves[i].firstFace );
      os.writeInt( leaves[i].nFaces );
      os.writeInt( leaves[i].cluster );
    }

    for( int i = 0; i < nLeafBrushes; ++i ) {
      os.writeInt( leafBrushes[i] );
    }

    for( int i = 0; i < nLeafFaces; ++i ) {
      os.writeInt( leafFaces[i] );
    }

    for( int i = 0; i < nBrushes; ++i ) {
      os.writeInt( brushes[i].firstSide );
      os.writeInt( brushes[i].nSides );
      os.writeInt( brushes[i].material );
    }

    for( int i = 0; i < nBrushSides; ++i ) {
      os.writeInt( brushSides[i] );
    }

    for( int i = 0; i < nModels; ++i ) {
      os.writeVec3( models[i].mins );
      os.writeVec3( models[i].maxs );
      os.writeInt( models[i].firstBrush );
      os.writeInt( models[i].nBrushes );
      os.writeInt( models[i].firstFace );
      os.writeInt( models[i].nFaces );
      os.writeVec3( models[i].move );
      os.writeFloat( models[i].ratioInc );
      os.writeInt( models[i].flags );
      os.writeChar( models[i].type );
      os.writeFloat( models[i].margin );
      os.writeFloat( models[i].slideTime );
      os.writeFloat( models[i].timeout );
    }

    for( int i = 0; i < nTextures; ++i ) {
      if( textures[i] == -1 ) {
        os.writePaddedString( "", 64 );
      }
      else {
        os.writePaddedString( translator.textures[ textures[i] ].name, 64 );
      }
    }

    for( int i = 0; i < nVertices; ++i ) {
      os.writeVec3( vertices[i].p );
      os.writeFloat( vertices[i].texCoord[0] );
      os.writeFloat( vertices[i].texCoord[1] );
      os.writeFloat( vertices[i].lightmapCoord[0] );
      os.writeFloat( vertices[i].lightmapCoord[0] );
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

    buffer.write( fileName );

    log.printEnd( " OK" );
    return true;
  }

#endif

  bool BSP::loadOZBSP( const char* fileName )
  {
    Buffer buffer;
    buffer.load( fileName );

    if( buffer.isEmpty() ) {
      log.println( "Cannot read file" );
      return false;
    }

    InputStream is = buffer.inputStream();

    mins         = is.readVec3();
    maxs         = is.readVec3();
    maxDim       = is.readFloat();
    life         = is.readFloat();

    nPlanes      = is.readInt();
    nNodes       = is.readInt();
    nLeaves      = is.readInt();
    nLeafBrushes = is.readInt();
    nLeafFaces   = is.readInt();
    nBrushes     = is.readInt();
    nBrushSides  = is.readInt();
    nModels      = is.readInt();
    nTextures    = is.readInt();
    nVertices    = is.readInt();
    nIndices     = is.readInt();
    nFaces       = is.readInt();
    nLightmaps   = is.readInt();

    int size = 0;

    size += nPlanes      * int( sizeof( Plane ) );
    size += nNodes       * int( sizeof( Node ) );
    size += nLeaves      * int( sizeof( Leaf ) );
    size += nLeafBrushes * int( sizeof( int ) );
    size += nLeafFaces   * int( sizeof( int ) );
    size += nBrushes     * int( sizeof( Brush ) );
    size += nBrushSides  * int( sizeof( int ) );
    size += nModels      * int( sizeof( Model ) );
    size += nTextures    * int( 64 * sizeof( char ) );
    size += nVertices    * int( sizeof( Vertex ) );
    size += nIndices     * int( sizeof( int ) );
    size += nFaces       * int( sizeof( Face ) );
    size += nLightmaps   * int( sizeof( Lightmap ) );

    char* data = Alloc::alloc<char>( size );

    planes = new( data ) Plane[nPlanes];
    for( int i = 0; i < nPlanes; ++i ) {
      planes[i].normal = is.readVec3();
      planes[i].distance = is.readFloat();
    }
    data += nPlanes * sizeof( Plane );

    nodes = new( data ) Node[nNodes];
    for( int i = 0; i < nNodes; ++i ) {
      nodes[i].plane = is.readInt();
      nodes[i].front = is.readInt();
      nodes[i].back = is.readInt();
    }
    data += nNodes * sizeof( Node );

    leaves = new( data ) Leaf[nLeaves];
    for( int i = 0; i < nLeaves; ++i ) {
      leaves[i].mins = is.readVec3();
      leaves[i].maxs = is.readVec3();
      leaves[i].firstBrush = is.readInt();
      leaves[i].nBrushes = is.readInt();
      leaves[i].firstFace = is.readInt();
      leaves[i].nFaces = is.readInt();
      leaves[i].cluster = is.readInt();
    }
    data += nLeaves * sizeof( Leaf );

    leafBrushes = new( data ) int[nLeafBrushes];
    for( int i = 0; i < nLeafBrushes; ++i ) {
      leafBrushes[i] = is.readInt();
    }
    data += nLeafBrushes * sizeof( int );

    leafFaces = new( data ) int[nLeafFaces];
    for( int i = 0; i < nLeafFaces; ++i ) {
      leafFaces[i] = is.readInt();
    }
    data += nLeafFaces * sizeof( int );

    brushes = new( data ) Brush[nBrushes];
    for( int i = 0; i < nBrushes; ++i ) {
      brushes[i].firstSide = is.readInt();
      brushes[i].nSides = is.readInt();
      brushes[i].material = is.readInt();
    }
    data += nBrushes * sizeof( Brush );

    brushSides = new( data ) int[nBrushSides];
    for( int i = 0; i < nBrushSides; ++i ) {
      brushSides[i] = is.readInt();
    }
    data += nBrushSides * sizeof( int );

    models = new( data ) Model[nModels];
    for( int i = 0; i < nModels; ++i ) {
      models[i].mins = is.readVec3();
      models[i].maxs = is.readVec3();
      models[i].firstBrush = is.readInt();
      models[i].nBrushes = is.readInt();
      models[i].firstFace = is.readInt();
      models[i].nFaces = is.readInt();
      models[i].move = is.readVec3();
      models[i].ratioInc = is.readFloat();
      models[i].flags = is.readInt();
      models[i].type = Model::Type( is.readChar() );
      models[i].margin = is.readFloat();
      models[i].slideTime = is.readFloat();
      models[i].timeout = is.readFloat();
    }
    data += nModels * sizeof( Model );

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

    vertices = new( data ) Vertex[nVertices];
    for( int i = 0; i < nVertices; ++i ) {
      vertices[i].p = is.readVec3();
      vertices[i].texCoord[0] = is.readFloat();
      vertices[i].texCoord[1] = is.readFloat();
      vertices[i].lightmapCoord[0] = is.readFloat();
      vertices[i].lightmapCoord[0] = is.readFloat();
    }
    data += nVertices * sizeof( Vertex );

    indices = new( data ) int[nIndices];
    for( int i = 0; i < nIndices; ++i ) {
      indices[i] = is.readInt();
    }
    data += nIndices * sizeof( int );

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

    lightmaps = new( data ) Lightmap[nLightmaps];
    for( int i = 0; i < nLightmaps; ++i ) {
      is.readChars( lightmaps[i].bits, LIGHTMAP_SIZE );
    }

    visual.nClusters = 0;
    visual.clusterLength = 0;
    visual.bitsets = null;

    return true;
  }

  BSP::BSP() : nPlanes( 0 ), nNodes( 0 ), nLeaves( 0 ), nLeafFaces( 0 ), nModels( 0 ),
      nBrushes( 0 ), nBrushSides( 0 ), nTextures( 0 ), nVertices( 0 ), nIndices( 0 ), nFaces( 0 ),
      nLightmaps( 0 ),
      planes( null ), nodes( null ), leaves( null ), leafFaces( null ), models( null ),
      brushes( null ), brushSides( null ), textures( null ), vertices( null ), indices( null ),
      faces( null ), lightmaps( null )
  {}

  BSP::~BSP()
  {
    free();
  }

  bool BSP::load( const char* name_ )
  {
    name = name_;

#ifdef OZ_USE_PREBUILT

    log.println( "Loading OpenZone BSP structure '%s' {", name.cstr() );
    log.indent();

    if( !loadOZBSP( "maps/" + name + ".ozBSP" ) ) {
      free();
      log.unindent();
      log.println();
    }

    log.unindent();
    log.println( "}" );

#else

    log.println( "Loading Quake 3 BSP structure '%s' {", name.cstr() );
    log.indent();

    if( !loadQBSP( "maps/" + name ) ) {
      free();
      log.unindent();
      log.println( "}" );
      return false;
    }

    optimise();

#ifdef OZ_MAKE_PREBUILT
    save( "maps/" + name + ".ozBSP" );
#endif

    log.unindent();
    log.println( "}" );

#endif

    return true;
  }

  void BSP::free()
  {
    log.print( "Freeing BSP structure '%s' ...", name.cstr() );

#ifdef OZ_USE_PREBUILT

    if( planes != null ) {
      aDestruct( planes, nPlanes );
      aDestruct( nodes, nNodes );
      aDestruct( leaves, nLeaves );
      aDestruct( leafBrushes, nLeafBrushes );
      aDestruct( leafFaces, nLeafFaces );
      aDestruct( models, nModels );
      aDestruct( brushes, nBrushes );
      aDestruct( brushSides, nBrushSides );
      aDestruct( textures, nTextures );
      aDestruct( vertices, nVertices );
      aDestruct( indices, nIndices );
      aDestruct( faces, nFaces );
      aDestruct( lightmaps, nLightmaps );

      Alloc::dealloc( planes );

      nPlanes      = 0;
      nNodes       = 0;
      nLeaves      = 0;
      nLeafBrushes = 0;
      nLeafFaces   = 0;
      nModels      = 0;
      nBrushes     = 0;
      nBrushSides  = 0;
      nTextures    = 0;
      nVertices    = 0;
      nIndices     = 0;
      nFaces       = 0;
      nLightmaps   = 0;

      planes      = null;
      nodes       = null;
      leaves      = null;
      leafBrushes = null;
      leafFaces   = null;
      models      = null;
      brushes     = null;
      brushSides  = null;
      textures    = null;
      vertices    = null;
      indices     = null;
      faces       = null;
      lightmaps   = null;
    }

#else

    if( planes != null ) {
      delete[] planes;
      nPlanes = 0;
      planes = null;
    }
    if( nodes != null ) {
      delete[] nodes;
      nNodes = 0;
      nodes = null;
    }
    if( leaves != null ) {
      delete[] leaves;
      nLeaves = 0;
      leaves = null;
    }
    if( leafBrushes != null ) {
      delete[] leafBrushes;
      nLeafBrushes = 0;
      leafBrushes = null;
    }
    if( leafFaces != null ) {
      delete[] leafFaces;
      nLeafFaces = 0;
      leafFaces = null;
    }
    if( models != null ) {
      delete[] models;
      nModels = 0;
      models = null;
    }
    if( brushes != null ) {
      delete[] brushes;
      nBrushes = 0;
      brushes = null;
    }
    if( brushSides != null ) {
      delete[] brushSides;
      nBrushSides = 0;
      brushSides = null;
    }
    if( textures != null ) {
      delete[] textures;
      nTextures = 0;
      textures = null;
    }
    if( vertices != null ) {
      delete[] vertices;
      nVertices = 0;
      vertices = null;
    }
    if( indices != null ) {
      delete[] indices;
      nIndices = 0;
      indices = null;
    }
    if( faces != null ) {
      delete[] faces;
      nFaces = 0;
      faces = null;
    }
    if( lightmaps != null ) {
      delete[] lightmaps;
      nLightmaps = 0;
      lightmaps = null;
    }

#endif

    log.printEnd( " OK" );
  }

}
