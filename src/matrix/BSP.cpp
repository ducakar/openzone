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
    enum Lumps : int
    {
      ENTITIES,
      TEXTURES,
      PLANES,
      NODES,
      LEAFS,
      LEAFFACES,
      LEAFBRUSHES,
      MODELS,
      BRUSHES,
      BRUSHSIDES,
      VERTICES,
      INDICES,
      SHADERS,
      FACES,
      LIGHTMAPS,
      LIGHTVOLUMES,
      VISUALDATA,
      MAX
    };

    int offset;
    int length;
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

      float offset = plane.abs() * Vec3( maxDim, maxDim, maxDim );

      if( offset <= plane.d ) {
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

    mins         = is.readPoint3();
    maxs         = is.readPoint3();
    life         = is.readFloat();

    nPlanes      = is.readInt();
    nNodes       = is.readInt();
    nLeaves      = is.readInt();
    nLeafBrushes = is.readInt();
    nBrushes     = is.readInt();
    nBrushSides  = is.readInt();
    nModels      = is.readInt();

    int size = 0;

    size += nPlanes      * int( sizeof( Plane ) );
    size += nNodes       * int( sizeof( Node ) );
    size += nLeaves      * int( sizeof( Leaf ) );
    size += nLeafBrushes * int( sizeof( int ) );
    size += nBrushes     * int( sizeof( Brush ) );
    size += nBrushSides  * int( sizeof( int ) );
    size = Alloc::alignUp( size );
    size += nModels      * int( sizeof( Model ) );

    char* data = Alloc::alloc<char>( size );

    planes = new( data ) Plane[nPlanes];
    for( int i = 0; i < nPlanes; ++i ) {
      planes[i] = is.readPlane();
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
      leaves[i].firstBrush = is.readInt();
      leaves[i].nBrushes = is.readInt();
    }
    data += nLeaves * sizeof( Leaf );

    leafBrushes = new( data ) int[nLeafBrushes];
    for( int i = 0; i < nLeafBrushes; ++i ) {
      leafBrushes[i] = is.readInt();
    }
    data += nLeafBrushes * sizeof( int );

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

    data = Alloc::alignUp( data );
    models = new( data ) Model[nModels];
    for( int i = 0; i < nModels; ++i ) {
      models[i].mins = is.readPoint3();
      models[i].maxs = is.readPoint3();
      models[i].bsp = this;
      models[i].firstBrush = is.readInt();
      models[i].nBrushes = is.readInt();
      models[i].move = is.readVec3();
      models[i].ratioInc = is.readFloat();
      models[i].flags = is.readInt();
      models[i].type = Model::Type( is.readInt() );
      models[i].margin = is.readFloat();
      models[i].slideTime = is.readFloat();
      models[i].timeout = is.readFloat();
    }

    hard_assert( !is.isAvailable() );

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
      aDestruct( models, nModels );
      aDestruct( brushes, nBrushes );
      aDestruct( brushSides, nBrushSides );

      Alloc::dealloc( planes );

      nPlanes      = 0;
      nNodes       = 0;
      nLeaves      = 0;
      nLeafBrushes = 0;
      nModels      = 0;
      nBrushes     = 0;
      nBrushSides  = 0;

      planes      = null;
      nodes       = null;
      leaves      = null;
      leafBrushes = null;
      models      = null;
      brushes     = null;
      brushSides  = null;
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
    float maxDim = bspConfig.get( "maxDim", Math::INF );
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

    QBSPLump lumps[QBSPLump::MAX];
    fread( lumps, sizeof( QBSPLump ), QBSPLump::MAX, file );

    int nTextures = int( lumps[QBSPLump::TEXTURES].length / sizeof( QBSPTexture ) );
    int* texFlags = new int[nTextures];
    int* texTypes = new int[nTextures];
    fseek( file, lumps[QBSPLump::TEXTURES].offset, SEEK_SET );

    for( int i = 0; i < nTextures; ++i ) {
      QBSPTexture texture;

      fread( &texture, sizeof( QBSPTexture ), 1, file );

      texFlags[i] = texture.flags;
      texTypes[i] = texture.type;
    }

    nPlanes = int( lumps[QBSPLump::PLANES].length / sizeof( QBSPPlane ) );
    planes = new Plane[nPlanes];
    fseek( file, lumps[QBSPLump::PLANES].offset, SEEK_SET );

    // rescale plane data
    for( int i = 0; i < nPlanes; ++i ) {
      QBSPPlane plane;

      fread( &plane, sizeof( QBSPPlane ), 1, file );

      planes[i]   = Vec3( plane.normal );
      planes[i].d = plane.distance * scale;

      float offset = planes[i].abs() * Vec3( maxDim, maxDim, maxDim );

      if( planes[i].d < -offset ) {
        planes[i].d = -Math::INF;
      }
      else if( planes[i].d > offset ) {
        planes[i].d = Math::INF;
      }
    }

    nNodes = int( lumps[QBSPLump::NODES].length / sizeof( QBSPNode ) );
    nodes = new BSP::Node[nNodes];
    fseek( file, lumps[QBSPLump::NODES].offset, SEEK_SET );

    for( int i = 0; i < nNodes; ++i ) {
      QBSPNode node;

      fread( &node, sizeof( QBSPNode ), 1, file );

      nodes[i].plane = node.plane;
      nodes[i].front = node.front;
      nodes[i].back  = node.back;
    }

    nLeaves = int( lumps[QBSPLump::LEAFS].length / sizeof( QBSPLeaf ) );
    leaves = new BSP::Leaf[nLeaves];
    fseek( file, lumps[QBSPLump::LEAFS].offset, SEEK_SET );

    for( int i = 0; i < nLeaves; ++i ) {
      QBSPLeaf leaf;

      fread( &leaf, sizeof( QBSPLeaf ), 1, file );

      leaves[i].firstBrush = leaf.firstBrush;
      leaves[i].nBrushes   = leaf.nBrushes;
    }

    nLeafBrushes = int( lumps[QBSPLump::LEAFBRUSHES].length / sizeof( int ) );
    leafBrushes = new int[nLeafBrushes];
    fseek( file, lumps[QBSPLump::LEAFBRUSHES].offset, SEEK_SET );
    fread( leafBrushes, sizeof( int ), nLeafBrushes, file );

    nModels = int( lumps[QBSPLump::MODELS].length / sizeof( QBSPModel ) ) - 1;
    models = null;

    if( nModels != 0 ) {
      models = new Model[nModels];
      fseek( file, lumps[QBSPLump::MODELS].offset, SEEK_SET );

      hard_assert( nModels <= 99 );
      char keyBuffer[] = "model  ";

      // skip model 0 (whole BSP)
      QBSPModel model;
      fread( &model, sizeof( QBSPModel ), 1, file );

      for( int i = 0; i < nModels; ++i ) {
        fread( &model, sizeof( QBSPModel ), 1, file );

        models[i].mins.x = model.bb[0][0] * scale;
        models[i].mins.y = model.bb[0][1] * scale;
        models[i].mins.z = model.bb[0][2] * scale;

        models[i].maxs.x = model.bb[1][0] * scale;
        models[i].maxs.y = model.bb[1][1] * scale;
        models[i].maxs.z = model.bb[1][2] * scale;

        models[i].mins -= Vec3( 2.0f * EPSILON, 2.0f * EPSILON, 2.0f * EPSILON );
        models[i].maxs += Vec3( 2.0f * EPSILON, 2.0f * EPSILON, 2.0f * EPSILON );

        models[i].bsp = this;

        models[i].firstBrush = model.firstBrush;
        models[i].nBrushes   = model.nBrushes;

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
          log.println( "invalid BSP entity type, should be either IGNORING, BLOCKING, PUSHING or "
              "CRUSHING" );
          delete[] texFlags;
          delete[] texTypes;
          return false;
        }

        models[i].margin = bspConfig.get( keyName + ".margin", 1.0f );
        models[i].slideTime = 1.0f;
        models[i].timeout = 5.0f;
      }
    }

    nBrushSides = int( lumps[QBSPLump::BRUSHSIDES].length / sizeof( QBSPBrushSide ) );
    brushSides = new int[nBrushSides];
    fseek( file, lumps[QBSPLump::BRUSHSIDES].offset, SEEK_SET );

    for( int i = 0; i < nBrushSides; ++i ) {
      QBSPBrushSide brushSide;

      fread( &brushSide, sizeof( QBSPBrushSide ), 1, file );

      brushSides[i] = brushSide.plane;
    }

    nBrushes = int( lumps[QBSPLump::BRUSHES].length / sizeof( QBSPBrush ) );
    brushes = new BSP::Brush[nBrushes];
    fseek( file, lumps[QBSPLump::BRUSHES].offset, SEEK_SET );

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
    delete[] models;
    delete[] brushes;
    delete[] brushSides;

    planes      = null;
    nodes       = null;
    leaves      = null;
    leafBrushes = null;
    models      = null;
    brushes     = null;
    brushSides  = null;

    nPlanes      = 0;
    nNodes       = 0;
    nLeaves      = 0;
    nLeafBrushes = 0;
    nModels      = 0;
    nBrushes     = 0;
    nBrushSides  = 0;

    log.printEnd( " OK" );
  }

  void BSP::optimise()
  {
    log.println( "Optimising BSP structure {" );
    log.indent();

    // remove brushes that lay out of boundaries
    for( int i = 0; i < nBrushes; ) {
      hard_assert( brushes[i].nSides >= 0 );

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
              hard_assert( leaves[k].nBrushes > 0 );

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
          hard_assert( models[j].nBrushes > 0 );

          --models[j].nBrushes;
        }
      }
      log.printEnd();
    }

    // remove model brushes from the static tree (Wtf Quake BSP puts them there in the first place?)
    log.print( "removing model brush references " );

    for( int i = 0; i < nModels; ++i ) {
      for( int j = 0; j < models[i].nBrushes; ++j ) {
        int brush = models[i].firstBrush + j;

        for( int k = 0; k < nLeafBrushes; ) {
          if( leafBrushes[k] != brush ) {
            ++k;
            continue;
          }

          aRemove( leafBrushes, k, nLeafBrushes );
          --nLeafBrushes;
          log.printRaw( "." );

          // adjust leaf references
          for( int l = 0; l < nLeaves; ++l ) {
            if( k < leaves[l].firstBrush ) {
              --leaves[l].firstBrush;
            }
            else if( k < leaves[l].firstBrush + leaves[l].nBrushes ) {
              hard_assert( leaves[l].nBrushes > 0 );

              --leaves[l].nBrushes;
            }
          }
        }
      }
    }

    log.printEnd( " OK" );

    // remove unreferenced leaves
    log.print( "removing unreferenced and empty leaves " );

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

    log.printEnd( " OK" );

    // collapse unnecessary nodes
    log.print( "collapsing nodes " );

    bool hasCollapsed;
    do {
      hasCollapsed = false;

      for( int i = 0; i < nNodes; ) {
        if( nodes[i].front != 0 && nodes[i].back != 0 ) {
          ++i;
          continue;
        }
        if( i == 0 ) {
          // change root node for one of its children, and set i to that child index, so it will
          // be removed instead of root
          if( nodes[0].front == 0 ) {
            i = nodes[0].back;
          }
          else if( nodes[0].back == 0 ) {
            i = nodes[0].front;
          }
          else {
            hard_assert( false );
          }
          nodes[0] = nodes[i];
        }
        else {
          // find parent
          int* parentsRef = null;
          for( int j = 0; j < nNodes; ++j ) {
            if( nodes[j].front == i ) {
              hard_assert( parentsRef == null );

              parentsRef = &nodes[j].front;
            }
            if( nodes[j].back == i ) {
              hard_assert( parentsRef == null );

              parentsRef = &nodes[j].back;
            }
          }
          hard_assert( parentsRef != null );

          if( nodes[i].front == 0 ) {
            *parentsRef = nodes[i].back;
          }
          else if( nodes[i].back == 0 ) {
            *parentsRef = nodes[i].front;
          }
          else {
            hard_assert( false );
          }
        }

        aRemove( nodes, i, nNodes );
        --nNodes;

        for( int j = 0; j < nNodes; ++j ) {
          hard_assert( nodes[j].front != i );
          hard_assert( nodes[j].back != i );
        }

        // shift nodes' references
        for( int j = 0; j < nNodes; ++j ) {
          if( nodes[j].front > i ) {
            --nodes[j].front;
          }
          if( nodes[j].back > i ) {
            --nodes[j].back;
          }
        }

        log.printRaw( "." );
        hasCollapsed = true;
      }
    }
    while( hasCollapsed );

    log.printEnd( " OK" );

    // remove unused brush sides
    log.print( "removing unused brush sides " );

    bool* usedBrushSides = new bool[nBrushSides];
    aSet( usedBrushSides, false, nBrushSides );

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
          hard_assert( false );
        }
      }
    }

    log.printEnd( " OK" );

    delete[] usedBrushSides;

    // remove unused planes
    log.print( "removing unused planes " );

    bool* usedPlanes = new bool[nPlanes];
    aSet( usedPlanes, false, nPlanes );

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
        hard_assert( nodes[j].plane != i );

        if( nodes[j].plane > i ) {
          --nodes[j].plane;
        }
      }
      for( int j = 0; j < nBrushSides; ++j ) {
        hard_assert( brushSides[j] != i );

        if( brushSides[j] > i ) {
          --brushSides[j];
        }
      }
    }

    delete[] usedPlanes;;

    log.printEnd( " OK" );

    // optimise bounds
    log.print( "Fitting bounds: " );

    mins = Point3( +Math::INF, +Math::INF, +Math::INF );
    maxs = Point3( -Math::INF, -Math::INF, -Math::INF );

    for( int i = 0; i < nBrushSides; ++i ) {
      Plane& plane = planes[ brushSides[i] ];

      if( plane.nx == -1.0f ) {
        mins.x = min( -plane.d, mins.x );
      }
      else if( plane.nx == 1.0f ) {
        maxs.x = max( +plane.d, maxs.x );
      }
      else if( plane.ny == -1.0f ) {
        mins.y = min( -plane.d, mins.y );
      }
      else if( plane.ny == 1.0f ) {
        maxs.y = max( +plane.d, maxs.y );
      }
      else if( plane.nz == -1.0f ) {
        mins.z = min( -plane.d, mins.z );
      }
      else if( plane.nz == 1.0f ) {
        maxs.z = max( +plane.d, maxs.z );
      }
    }

    log.printEnd( "(%g %g %g) (%g %g %g)", mins.x, mins.y, mins.z, maxs.x, maxs.y, maxs.z );

    log.unindent();
    log.println( "}" );
  }

  void BSP::check( bool isOptimised ) const
  {
    log.print( "Integrity check ..." );

    Bitset usedNodes( nNodes );
    Bitset usedLeaves( nLeaves );
    Bitset usedBrushes( nBrushes );

    usedNodes.clearAll();
    usedLeaves.clearAll();
    usedBrushes.clearAll();

    for( int i = 0; i < nNodes; ++i ) {
      if( nodes[i].front < 0 ) {
        if( usedLeaves.get( ~nodes[i].front ) ) {
          throw Exception( "BSP leaf " + String( ~nodes[i].front ) + " referenced twice" );
        }
        usedLeaves.set( ~nodes[i].front );
      }
      else if( nodes[i].front != 0 ) {
        if( usedNodes.get( nodes[i].front ) ) {
          throw Exception( "BSP node " + String( nodes[i].front ) + " referenced twice" );
        }
        usedNodes.set( nodes[i].front );
      }
      else {
        throw Exception( "BSP root node referenced" );
      }

      if( nodes[i].back < 0 ) {
        if( usedLeaves.get( ~nodes[i].back ) ) {
          throw Exception( "BSP leaf " + String( ~nodes[i].back ) + " referenced twice" );
        }
        usedLeaves.set( ~nodes[i].back );
      }
      else if( nodes[i].back != 0 ) {
        if( usedNodes.get( nodes[i].back ) ) {
          throw Exception( "BSP node " + String( nodes[i].back ) + " referenced twice" );
        }
        usedNodes.set( nodes[i].back );
      }
      else {
        throw Exception( "BSP root node referenced" );
      }
    }

    for( int i = 0; i < nModels; ++i ) {
      for( int j = 0; j < models[i].nBrushes; ++j ) {
        int index = models[i].firstBrush + j;

        if( usedBrushes.get( index ) ) {
          throw Exception( "BSP brush " + String( index ) + " referenced by two models" );
        }
        usedBrushes.set( index );
      }
    }

    usedBrushes.clearAll();
    for( int i = 0; i < nLeaves; ++i ) {
      for( int j = 0; j < leaves[i].nBrushes; ++j ) {
        int index = leafBrushes[ leaves[i].firstBrush + j ];

        usedBrushes.set( index );
      }
    }
    for( int i = 0; i < nModels; ++i ) {
      for( int j = 0; j < models[i].nBrushes; ++j ) {
        int index = models[i].firstBrush + j;

        if( isOptimised && usedBrushes.get( index ) ) {
          throw Exception( "BSP model brush " + String( index ) + " referenced by static tree" );
        }
        usedBrushes.set( index );
      }
    }

    if( usedNodes.get( 0 ) ) {
      throw Exception( "BSP root node referenced" );
    }
    for( int i = 1; i < nNodes; ++i ) {
      if( !usedNodes.get( i ) ) {
        throw Exception( "BSP node " + String( i ) + " not referenced" );
      }
    }
    for( int i = 0; i < nLeaves; ++i ) {
      if( isOptimised && !usedLeaves.get( i ) ) {
        throw Exception( "BSP leaf " + String( i ) + " not referenced" );
      }
    }
    for( int i = 0; i < nBrushes; ++i ) {
      if( !usedBrushes.get( i ) ) {
        throw Exception( "BSP brush " + String( i ) + " not referenced" );
      }
    }

    for( int i = 0; i < nPlanes; ++i ) {
      if( !Math::isFinite( planes[i].d ) ) {
        throw Exception( "BSP has invalid plane " + String( i ) );
      }
    }

    log.printEnd( " OK" );
  }

  bool BSP::save( const char* fileName )
  {
    log.print( "Dumping BSP structure to '%s' ...", fileName );

    int size = 0;

    size += 1            * int( sizeof( Bounds ) );
    size += 1            * int( sizeof( float ) );
    size += 7            * int( sizeof( int ) );
    size += nPlanes      * int( sizeof( Plane ) );
    size += nNodes       * int( sizeof( Node ) );
    size += nLeaves      * int( sizeof( Leaf ) );
    size += nLeafBrushes * int( sizeof( int ) );
    size += nBrushes     * int( sizeof( Brush ) );
    size += nBrushSides  * int( sizeof( int ) );
    size += nModels      * int( sizeof( Model ) - sizeof( void* ) );

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
    os.writeInt( nModels );

    for( int i = 0; i < nPlanes; ++i ) {
      os.writePlane( planes[i] );
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

    for( int i = 0; i < nModels; ++i ) {
      os.writePoint3( models[i].mins );
      os.writePoint3( models[i].maxs );
      os.writeInt( models[i].firstBrush );
      os.writeInt( models[i].nBrushes );
      os.writeVec3( models[i].move );
      os.writeFloat( models[i].ratioInc );
      os.writeInt( models[i].flags );
      os.writeInt( int( models[i].type ) );
      os.writeFloat( models[i].margin );
      os.writeFloat( models[i].slideTime );
      os.writeFloat( models[i].timeout );
    }

    hard_assert( !os.isAvailable() );
    buffer.write( fileName );

    log.printEnd( " OK" );
    return true;
  }

  BSP::BSP() :
      nPlanes( 0 ), nNodes( 0 ), nLeaves( 0 ), nLeafBrushes( 0 ), nModels( 0 ),
      nBrushes( 0 ), nBrushSides( 0 ),
      planes( null ), nodes( null ), leaves( null ), leafBrushes( null ), models( null ),
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
    bsp->check( true );
    bsp->save( String( "maps/" ) + name + String( ".ozBSP" ) );
    bsp->freeQBSP();
    delete bsp;

    log.unindent();
    log.println( "}" );
  }

  BSP::BSP( const char* name_ ) :
      name( name_ ),
      nPlanes( 0 ), nNodes( 0 ), nLeaves( 0 ), nLeafBrushes( 0 ), nModels( 0 ),
      nBrushes( 0 ), nBrushSides( 0 ),
      planes( null ), nodes( null ), leaves( null ), leafBrushes( null ), models( null ),
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
