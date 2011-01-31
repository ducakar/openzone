/*
 *  BSP.cpp
 *
 *  Data structure for Quake3 BSP level
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
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

  inline bool BSP::includes( const BSP::Brush& brush, float maxDim ) const
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

  bool BSP::loadOZBSP( const char* fileName )
  {
    Buffer buffer;
    buffer.read( fileName );

    if( buffer.isEmpty() ) {
      return false;
    }

    InputStream is = buffer.inputStream();

    mins           = is.readPoint3();
    maxs           = is.readPoint3();
    life           = is.readFloat();

    nPlanes        = is.readInt();
    nNodes         = is.readInt();
    nLeaves        = is.readInt();
    nLeafBrushes   = is.readInt();
    nBrushes       = is.readInt();
    nBrushSides    = is.readInt();
    nEntityClasses = is.readInt();

    int size = 0;

    size += nPlanes        * int( sizeof( Plane ) );
    size = Alloc::alignUp( size );

    size += nNodes         * int( sizeof( Node ) );
    size = Alloc::alignUp( size );

    size += nLeaves        * int( sizeof( Leaf ) );
    size = Alloc::alignUp( size );

    size += nLeafBrushes   * int( sizeof( int ) );
    size = Alloc::alignUp( size );

    size += nBrushes       * int( sizeof( Brush ) );
    size = Alloc::alignUp( size );

    size += nBrushSides    * int( sizeof( int ) );
    size = Alloc::alignUp( size );

    size += nEntityClasses * int( sizeof( EntityClass ) );
    size = Alloc::alignUp( size );

    char* data = Alloc::alloc<char>( size );

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
      leaves[i].firstBrush = is.readInt();
      leaves[i].nBrushes = is.readInt();
    }
    data += nLeaves * sizeof( Leaf );
    data = Alloc::alignUp( data );

    leafBrushes = new( data ) int[nLeafBrushes];
    for( int i = 0; i < nLeafBrushes; ++i ) {
      leafBrushes[i] = is.readInt();
    }
    data += nLeafBrushes * sizeof( int );
    data = Alloc::alignUp( data );

    brushes = new( data ) Brush[nBrushes];
    for( int i = 0; i < nBrushes; ++i ) {
      brushes[i].firstSide = is.readInt();
      brushes[i].nSides = is.readInt();
      brushes[i].material = is.readInt();
    }
    data += nBrushes * sizeof( Brush );
    data = Alloc::alignUp( data );

    brushSides = new( data ) int[nBrushSides];
    for( int i = 0; i < nBrushSides; ++i ) {
      brushSides[i] = is.readInt();
    }
    data += nBrushSides * sizeof( int );
    data = Alloc::alignUp( data );

    entityClasses = new( data ) EntityClass[nEntityClasses];
    for( int i = 0; i < nEntityClasses; ++i ) {
      entityClasses[i].mins = is.readPoint3();
      entityClasses[i].maxs = is.readPoint3();
      entityClasses[i].bsp = this;
      entityClasses[i].firstBrush = is.readInt();
      entityClasses[i].nBrushes = is.readInt();
      entityClasses[i].move = is.readVec3();
      entityClasses[i].ratioInc = is.readFloat();
      entityClasses[i].flags = is.readInt();
      entityClasses[i].mode = EntityClass::Mode( is.readChar() );
      entityClasses[i].margin = is.readFloat();
      entityClasses[i].slideTime = is.readFloat();
      entityClasses[i].timeout = is.readFloat();
    }
    data += nEntityClasses * sizeof( EntityClass );
    data = Alloc::alignUp( data );

    return true;
  }

  void BSP::freeOZBSP()
  {
    log.print( "Freeing BSP structure '%s' ...", name.cstr() );

    if( planes != null ) {
      aDestruct( planes, nPlanes );
      aDestruct( nodes, nNodes );
      aDestruct( leaves, nLeaves );
      aDestruct( leafBrushes, nLeafBrushes );
      aDestruct( entityClasses, nEntityClasses );
      aDestruct( brushes, nBrushes );
      aDestruct( brushSides, nBrushSides );

      Alloc::dealloc( planes );

      nPlanes        = 0;
      nNodes         = 0;
      nLeaves        = 0;
      nLeafBrushes   = 0;
      nEntityClasses = 0;
      nBrushes       = 0;
      nBrushSides    = 0;

      planes        = null;
      nodes         = null;
      leaves        = null;
      leafBrushes   = null;
      entityClasses = null;
      brushes       = null;
      brushSides    = null;
    }

    log.printEnd( " OK" );
  }

  bool BSP::loadQBSP( const char* fileName )
  {
    // TODO move cfg stuff to prebuild()
    String rcFile = fileName + String( ".rc" );
    String bspFile = fileName + String( ".bsp" );

    Config bspConfig;
    if( !bspConfig.load( rcFile ) ) {
      return false;
    }

    float scale = bspConfig.get( "scale", 0.01f );
    float maxDim = bspConfig.get( "maxDim", Math::inf() );
    life = bspConfig.get( "life", 1000.0f );

    mins = Point3( -maxDim, -maxDim, -maxDim );
    maxs = Point3( +maxDim, +maxDim, +maxDim );

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

    int nTextures = int( lumps[QBSP_LUMP_TEXTURES].length / sizeof( QBSPTexture ) );
    int* texFlags = new int[nTextures];
    int* texTypes = new int[nTextures];
    fseek( file, lumps[QBSP_LUMP_TEXTURES].offset, SEEK_SET );

    for( int i = 0; i < nTextures; ++i ) {
      QBSPTexture texture;

      fread( &texture, sizeof( QBSPTexture ), 1, file );

      texFlags[i] = texture.flags;
      texTypes[i] = texture.type;
    }

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

      leaves[i].firstBrush = leaf.firstBrush;
      leaves[i].nBrushes   = leaf.nBrushes;
    }

    nLeafBrushes = int( lumps[QBSP_LUMP_LEAFBRUSHES].length / sizeof( int ) );
    leafBrushes = new int[nLeafBrushes];
    fseek( file, lumps[QBSP_LUMP_LEAFBRUSHES].offset, SEEK_SET );
    fread( leafBrushes, sizeof( int ), nLeafBrushes, file );

    nEntityClasses = int( lumps[QBSP_LUMP_MODELS].length / sizeof( QBSPModel ) );
    entityClasses = new EntityClass[nEntityClasses];
    fseek( file, lumps[QBSP_LUMP_MODELS].offset, SEEK_SET );

    if( nEntityClasses < 1 ) {
      log.println( "BSP should contain at least 1 model (entire BSP)" );
      return false;
    }

    assert( nEntityClasses <= 99 );
    char keyBuffer[] = "model  ";

    for( int i = 0; i < nEntityClasses; ++i ) {
      QBSPModel model;

      fread( &model, sizeof( QBSPModel ), 1, file );

      entityClasses[i].mins.x = model.bb[0][0] * scale;
      entityClasses[i].mins.y = model.bb[0][1] * scale;
      entityClasses[i].mins.z = model.bb[0][2] * scale;

      entityClasses[i].maxs.x = model.bb[1][0] * scale;
      entityClasses[i].maxs.y = model.bb[1][1] * scale;
      entityClasses[i].maxs.z = model.bb[1][2] * scale;

      entityClasses[i].mins -= Vec3( 2.0f * EPSILON, 2.0f * EPSILON, 2.0f * EPSILON );
      entityClasses[i].maxs += Vec3( 2.0f * EPSILON, 2.0f * EPSILON, 2.0f * EPSILON );

      entityClasses[i].bsp = this;

      entityClasses[i].firstBrush = model.firstBrush;
      entityClasses[i].nBrushes   = model.nBrushes;

      keyBuffer[5] = char( '0' + i / 10 );
      keyBuffer[6] = char( '0' + i % 10 );
      String keyName = keyBuffer;

      entityClasses[i].move.x = bspConfig.get( keyName + ".move.x", 0.0f );
      entityClasses[i].move.y = bspConfig.get( keyName + ".move.y", 0.0f );
      entityClasses[i].move.z = bspConfig.get( keyName + ".move.z", 0.0f );

      entityClasses[i].ratioInc = bspConfig.get( keyName + ".ratioInc", Timer::TICK_TIME );
      entityClasses[i].flags = 0;

      String type = bspConfig.get( keyName + ".type", "BLOCKING" );
      if( type.equals( "IGNORING" ) ) {
        entityClasses[i].mode = EntityClass::IGNORING;
      }
      else if( type.equals( "BLOCKING" ) ) {
        entityClasses[i].mode = EntityClass::BLOCKING;
      }
      else if( type.equals( "PUSHING" ) ) {
        entityClasses[i].mode = EntityClass::PUSHING;
      }
      else if( type.equals( "CRUSHING" ) ) {
        entityClasses[i].mode = EntityClass::CRUSHING;
      }
      else {
        log.println( "invalid BSP model type, should be either IGNORING, BLOCKING, PUSHING or "
            "CRUSHING" );
        delete[] texFlags;
        delete[] texTypes;
        return false;
      }

      entityClasses[i].margin = bspConfig.get( keyName + ".margin", 1.0f );
      entityClasses[i].slideTime = 1.0f;
      entityClasses[i].timeout = 5.0f;
    }

    entityClasses[0].mins = mins;
    entityClasses[0].maxs = maxs;

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
      if( !includes( brushes[i], maxDim ) ) {
        brushes[i].nSides = 0;
      }
    }

    if( nBrushes > MAX_BRUSHES ) {
      log.println( "Too many brushes %d, maximum is %d", nBrushes, MAX_BRUSHES );
      delete[] texFlags;
      delete[] texTypes;
      return false;
    }

    delete[] texFlags;
    delete[] texTypes;

    fclose( file );

    return true;
  }

  void BSP::freeQBSP()
  {
    log.print( "Freeing Quake 3 BSP structure '%s' ...", name.cstr() );

    delete[] planes;
    delete[] nodes;
    delete[] leaves;
    delete[] leafBrushes;
    delete[] entityClasses;
    delete[] brushes;
    delete[] brushSides;

    planes        = null;
    nodes         = null;
    leaves        = null;
    leafBrushes   = null;
    entityClasses = null;
    brushes       = null;
    brushSides    = null;

    nPlanes        = 0;
    nNodes         = 0;
    nLeaves        = 0;
    nLeafBrushes   = 0;
    nEntityClasses = 0;
    nBrushes       = 0;
    nBrushSides    = 0;

    log.printEnd( " OK" );
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
      for( int j = 0; j < nEntityClasses; ++j ) {
        if( i < entityClasses[j].firstBrush ) {
          --entityClasses[j].firstBrush;
        }
        else if( i < entityClasses[j].firstBrush + entityClasses[j].nBrushes ) {
          assert( entityClasses[j].nBrushes > 0 );

          --entityClasses[j].nBrushes;
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
          break;
        }
      }

      if( isReferenced && leaves[i].nBrushes != 0 ) {
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

    mins = Point3( +Math::inf(), +Math::inf(), +Math::inf() );
    maxs = Point3( -Math::inf(), -Math::inf(), -Math::inf() );

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

    entityClasses[0].mins = mins;
    entityClasses[0].maxs = maxs;

    log.printEnd( "(%g %g %g) (%g %g %g)", mins.x, mins.y, mins.z, maxs.x, maxs.y, maxs.z );

    log.unindent();
    log.println( "}" );
  }

  bool BSP::save( const char* fileName )
  {
    log.print( "Dumping BSP structure to '%s' ...", fileName );

    int size = 0;

    size += 1              * int( sizeof( Bounds ) );
    size += 1              * int( sizeof( float ) );
    size += 7              * int( sizeof( int ) );
    size += nPlanes        * int( sizeof( Plane ) );
    size += nNodes         * int( sizeof( Node ) );
    size += nLeaves        * int( sizeof( Leaf ) );
    size += nLeafBrushes   * int( sizeof( int ) );
    size += nBrushes       * int( sizeof( Brush ) );
    size += nBrushSides    * int( sizeof( int ) );
    size += nEntityClasses * int( sizeof( EntityClass ) );

    Buffer buffer( size );
    OutputStream os = buffer.outputStream();

    os.writePoint3( mins );
    os.writePoint3( maxs );
    os.writeFloat( life );

    os.writeInt( nPlanes );
    os.writeInt( nNodes );
    os.writeInt( nLeaves );
    os.writeInt( nLeafBrushes );
    os.writeInt( nBrushes );
    os.writeInt( nBrushSides );
    os.writeInt( nEntityClasses );

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
      os.writeInt( leaves[i].firstBrush );
      os.writeInt( leaves[i].nBrushes );
    }

    for( int i = 0; i < nLeafBrushes; ++i ) {
      os.writeInt( leafBrushes[i] );
    }

    for( int i = 0; i < nBrushes; ++i ) {
      os.writeInt( brushes[i].firstSide );
      os.writeInt( brushes[i].nSides );
      os.writeInt( brushes[i].material );
    }

    for( int i = 0; i < nBrushSides; ++i ) {
      os.writeInt( brushSides[i] );
    }

    for( int i = 0; i < nEntityClasses; ++i ) {
      os.writePoint3( entityClasses[i].mins );
      os.writePoint3( entityClasses[i].maxs );
      os.writeInt( entityClasses[i].firstBrush );
      os.writeInt( entityClasses[i].nBrushes );
      os.writeVec3( entityClasses[i].move );
      os.writeFloat( entityClasses[i].ratioInc );
      os.writeInt( entityClasses[i].flags );
      os.writeChar( entityClasses[i].mode );
      os.writeFloat( entityClasses[i].margin );
      os.writeFloat( entityClasses[i].slideTime );
      os.writeFloat( entityClasses[i].timeout );
    }

    buffer.write( fileName );

    log.printEnd( " OK" );
    return true;
  }

  BSP::BSP() :
      nPlanes( 0 ), nNodes( 0 ), nLeaves( 0 ), nLeafBrushes( 0 ), nEntityClasses( 0 ),
      nBrushes( 0 ), nBrushSides( 0 ),
      planes( null ), nodes( null ), leaves( null ), leafBrushes( null ), entityClasses( null ),
      brushes( null ), brushSides( null )
  {}

  void BSP::prebuild( const char* name )
  {
    log.println( "Prebuilding Quake 3 BSP structure '%s' {", name );
    log.indent();

    BSP* bsp = new BSP();
    bsp->name = name;

    if( !bsp->loadQBSP( String( "maps/" ) + name ) ) {
      bsp->freeQBSP();
      log.unindent();
      log.println( "}" );
      throw Exception( "Matrix QBSP loading failed" );
    }

    bsp->optimise();
    bsp->save( String( "maps/" ) + name + String( ".ozBSP" ) );
    bsp->freeQBSP();
    delete bsp;

    log.unindent();
    log.println( "}" );
  }

  BSP::BSP( const char* name_ ) :
      name( name_ ),
      nPlanes( 0 ), nNodes( 0 ), nLeaves( 0 ), nLeafBrushes( 0 ), nEntityClasses( 0 ),
      nBrushes( 0 ), nBrushSides( 0 ),
      planes( null ), nodes( null ), leaves( null ), leafBrushes( null ), entityClasses( null ),
      brushes( null ), brushSides( null )
  {
    log.print( "Loading OpenZone BSP structure '%s' ...", name.cstr() );

    if( !loadOZBSP( "maps/" + name + ".ozBSP" ) ) {
      log.printEnd( " Failed" );
      freeOZBSP();
      throw Exception( "Matrix ozBSP loading failed" );
    }

    log.printEnd( " OK" );
  }

  BSP::~BSP()
  {
    freeOZBSP();
  }

}
