/*
 *  BSP.cpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

/**
 * @file build.BSP.cpp
 */

#include "stable.hpp"

#include "build/BSP.hpp"

#include "matrix/Collider.hpp"
#include "matrix/BSP.hpp"
#include "matrix/Library.hpp"

#include "client/Compiler.hpp"

using namespace oz::client;

namespace oz
{
namespace build
{

const float BSP::DEFAULT_SCALE      = 0.01f;
const float BSP::DEFAULT_LIFE       = 10000.0f;
const float BSP::DEFAULT_RESISTANCE = 400.0f;

inline bool BSP::includes( const oz::BSP::Brush& brush, float maxDim ) const
{
  for( int i = 0; i < brush.nSides; ++i ) {
    const Plane& plane = planes[ brushSides[brush.firstSide + i] ];

    float offset = Vec3( maxDim, maxDim, maxDim ) * plane.abs();

    if( offset <= plane.d ) {
      return false;
    }
  }
  return true;
}

void BSP::load()
{
  String rcFile = "data/maps/" + name + String( ".rc" );
  String bspFile = "data/maps/" + name + String( ".bsp" );

  Config bspConfig;
  if( !bspConfig.load( rcFile ) ) {
    throw Exception( "BSP config loading failed" );
  }

  float scale = bspConfig.get( "scale", DEFAULT_SCALE );
  float maxDim = bspConfig.get( "maxDim", Math::INF );

  life = bspConfig.get( "life", DEFAULT_LIFE );
  resistance = bspConfig.get( "resistance", DEFAULT_RESISTANCE );

  mins = Point3( -maxDim, -maxDim, -maxDim );
  maxs = Point3( +maxDim, +maxDim, +maxDim );

  if( Math::isNaN( scale ) || Math::isNaN( maxDim ) ) {
    throw Exception( "Invalid BSP config" );
  }

  Buffer buffer;
  if( !buffer.read( bspFile ) ) {
    throw Exception( "BSP reading failed" );
  }

  InputStream istream = buffer.inputStream();

  char id[4];
  id[0] = istream.readChar();
  id[1] = istream.readChar();
  id[2] = istream.readChar();
  id[3] = istream.readChar();

  int version = istream.readInt();

  if( id[0] != 'I' || id[1] != 'B' || id[2] != 'S' || id[3] != 'P' || version != 46 ) {
    throw Exception( "Wrong Quake 3 BSP format" );
  }

  DArray<QBSPLump> lumps( QBSPLump::MAX );
  for( int i = 0; i < QBSPLump::MAX; ++i ) {
    lumps[i].offset = istream.readInt();
    lumps[i].length = istream.readInt();
  }

  nTextures = lumps[QBSPLump::TEXTURES].length / int( sizeof( QBSPTexture ) );
  textures = new Texture[nTextures];

  istream.reset();
  istream.skip( lumps[QBSPLump::TEXTURES].offset );

  for( int i = 0; i < nTextures; ++i ) {
    String name = istream.prepareRead( 64 );

    textures[i].flags = istream.readInt();
    textures[i].type  = istream.readInt();

    if( name.length() < 10 ) {
      textures[i].id = -1;
    }
    else {
      name = name.substring( 9 );

      if( name.equals( "NULL" ) || ( textures[i].flags & QBSP_LADDER_FLAG_BIT ) ) {
        textures[i].id = -1;
      }
      else {
        textures[i].id = library.textureIndex( name );
      }
    }

    log.println( "Texture '%s' flags %x type %x",
                 name.cstr(),
                 textures[i].flags,
                 textures[i].type );
  }

  nPlanes = lumps[QBSPLump::PLANES].length / int( sizeof( QBSPPlane ) );
  planes = new Plane[nPlanes];

  istream.reset();
  istream.skip( lumps[QBSPLump::PLANES].offset );

  for( int i = 0; i < nPlanes; ++i ) {
    planes[i].nx = istream.readFloat();
    planes[i].ny = istream.readFloat();
    planes[i].nz = istream.readFloat();
    planes[i].d  = istream.readFloat() * scale;
  }

  nNodes = lumps[QBSPLump::NODES].length / int( sizeof( QBSPNode ) );
  nodes = new oz::BSP::Node[nNodes];

  istream.reset();
  istream.skip( lumps[QBSPLump::NODES].offset );

  for( int i = 0; i < nNodes; ++i ) {
    nodes[i].plane = istream.readInt();
    nodes[i].front = istream.readInt();
    nodes[i].back  = istream.readInt();

    // int bb[2][3]
    istream.readInt();
    istream.readInt();
    istream.readInt();
    istream.readInt();
    istream.readInt();
    istream.readInt();
  }

  nLeaves = lumps[QBSPLump::LEAFS].length / int( sizeof( QBSPLeaf ) );
  leaves = new oz::BSP::Leaf[nLeaves];

  istream.reset();
  istream.skip( lumps[QBSPLump::LEAFS].offset );

  for( int i = 0; i < nLeaves; ++i ) {
    // int cluster
    istream.readInt();
    // int area
    istream.readInt();
    // int bb[2][3]
    istream.readInt();
    istream.readInt();
    istream.readInt();
    istream.readInt();
    istream.readInt();
    istream.readInt();
    // int firstFace
    istream.readInt();
    // int nFaces
    istream.readInt();

    leaves[i].firstBrush = istream.readInt();
    leaves[i].nBrushes   = istream.readInt();
  }

  nLeafBrushes = lumps[QBSPLump::LEAFBRUSHES].length / int( sizeof( int ) );
  leafBrushes = new int[nLeafBrushes];

  istream.reset();
  istream.skip( lumps[QBSPLump::LEAFBRUSHES].offset );

  for( int i = 0; i < nLeafBrushes; ++i ) {
    leafBrushes[i] = istream.readInt();
  }

  nModels = lumps[QBSPLump::MODELS].length / int( sizeof( QBSPModel ) ) - 1;
  models = null;
  modelFaces = new ModelFaces[nModels + 1];

  if( nModels != 0 ) {
    models = new oz::BSP::Model[nModels];

    istream.reset();
    istream.skip( lumps[QBSPLump::MODELS].offset );

    hard_assert( nModels <= 99 );
    char keyBuffer[] = "model  ";

    // skip model 0 (whole BSP)
    istream.skip( int( sizeof( QBSPModel ) ) );

    for( int i = 0; i < nModels; ++i ) {
      models[i].mins.x = istream.readFloat() * scale - 4.0f * EPSILON;
      models[i].mins.y = istream.readFloat() * scale - 4.0f * EPSILON;
      models[i].mins.z = istream.readFloat() * scale - 4.0f * EPSILON;

      models[i].maxs.x = istream.readFloat() * scale + 4.0f * EPSILON;
      models[i].maxs.y = istream.readFloat() * scale + 4.0f * EPSILON;
      models[i].maxs.z = istream.readFloat() * scale + 4.0f * EPSILON;

      // int firstFace
      istream.readInt();
      // int nFaces
      istream.readInt();

      models[i].firstBrush = istream.readInt();
      models[i].nBrushes   = istream.readInt();

      keyBuffer[5] = char( '0' + i / 10 );
      keyBuffer[6] = char( '0' + i % 10 );
      String keyName = keyBuffer;

      models[i].move.x = bspConfig.get( keyName + ".move.x", 0.0f );
      models[i].move.y = bspConfig.get( keyName + ".move.y", 0.0f );
      models[i].move.z = bspConfig.get( keyName + ".move.z", 0.0f );

      models[i].ratioInc = Timer::TICK_TIME / bspConfig.get( keyName + ".slideTime", 1.0f );
      models[i].flags    = 0;

      String type = bspConfig.get( keyName + ".type", "BLOCKING" );
      if( type.equals( "IGNORING" ) ) {
        models[i].type = oz::BSP::Model::IGNORING;
      }
      else if( type.equals( "CRUSHING" ) ) {
        models[i].type = oz::BSP::Model::CRUSHING;
      }
      else if( type.equals( "AUTO_DOOR" ) ) {
        models[i].type = oz::BSP::Model::AUTO_DOOR;
      }
      else {
        throw Exception( "Invalid BSP model type, must be either IGNORING, CRUSHING or AUTO_DOOR" );
      }

      models[i].margin  = bspConfig.get( keyName + ".margin", 1.0f );
      models[i].timeout = bspConfig.get( keyName + ".timeout", 6.0f );

      String sOpenSample  = bspConfig.get( keyName + ".openSample", "" );
      String sCloseSample = bspConfig.get( keyName + ".closeSample", "" );
      String sFrictSample = bspConfig.get( keyName + ".frictSample", "" );

      models[i].openSample  = sOpenSample.isEmpty()  ? -1 : library.soundIndex( sOpenSample );
      models[i].closeSample = sCloseSample.isEmpty() ? -1 : library.soundIndex( sCloseSample );
      models[i].frictSample = sFrictSample.isEmpty() ? -1 : library.soundIndex( sFrictSample );
    }
  }

  istream.reset();
  istream.skip( lumps[QBSPLump::MODELS].offset );

  for( int i = 0; i < nModels + 1; ++i ) {
    // float bb[2][3]
    istream.readFloat();
    istream.readFloat();
    istream.readFloat();
    istream.readFloat();
    istream.readFloat();
    istream.readFloat();

    modelFaces[i].firstFace = istream.readInt();
    modelFaces[i].nFaces    = istream.readInt();

    // int firstBrush
    istream.readInt();
    // int nBrushes
    istream.readInt();
  }

  nBrushSides = lumps[QBSPLump::BRUSHSIDES].length / int( sizeof( QBSPBrushSide ) );
  brushSides = new int[nBrushSides];

  istream.reset();
  istream.skip( lumps[QBSPLump::BRUSHSIDES].offset );

  for( int i = 0; i < nBrushSides; ++i ) {
    brushSides[i] = istream.readInt();

    // int texture
    istream.readInt();
  }

  nBrushes = lumps[QBSPLump::BRUSHES].length / int( sizeof( QBSPBrush ) );
  brushes = new oz::BSP::Brush[nBrushes];

  if( nBrushes > oz::BSP::MAX_BRUSHES ) {
    throw Exception( "Too many brushes " + String( nBrushes ) + ", can be at most " +
                     String( oz::BSP::MAX_BRUSHES ) );
  }

  istream.reset();
  istream.skip( lumps[QBSPLump::BRUSHES].offset );

  for( int i = 0; i < nBrushes; ++i ) {
    brushes[i].firstSide = istream.readInt();
    brushes[i].nSides    = istream.readInt();
    brushes[i].material  = 0;

    int texture = istream.readInt();

    // brush out of bounds, mark it for exclusion
    if( !includes( brushes[i], maxDim ) ) {
      brushes[i].nSides = 0;
      continue;
    }

    if( textures[texture].flags & QBSP_LADDER_FLAG_BIT ) {
      brushes[i].material |= Material::LADDER_BIT;
    }
    if( !( textures[texture].flags & QBSP_NONSOLID_FLAG_BIT ) ) {
      brushes[i].material |= Material::STRUCT_BIT;
    }
    if( textures[texture].flags & QBSP_SLICK_FLAG_BIT ) {
      brushes[i].material |= Material::SLICK_BIT;
    }
    if( textures[texture].type & QBSP_WATER_TYPE_BIT ) {
      brushes[i].material |= Material::WATER_BIT;
    }
  }

  nVertices = lumps[QBSPLump::VERTICES].length / int( sizeof( QBSPVertex ) );
  vertices = new client::Vertex[nVertices];

  istream.reset();
  istream.skip( lumps[QBSPLump::VERTICES].offset );

  for( int i = 0; i < nVertices; ++i ) {
    vertices[i].pos[0]      = istream.readFloat() * scale;
    vertices[i].pos[1]      = istream.readFloat() * scale;
    vertices[i].pos[2]      = istream.readFloat() * scale;

    vertices[i].texCoord[0] = istream.readFloat();
    vertices[i].texCoord[1] = 1.0f - istream.readFloat();

    // float lightmapCoord[2]
    istream.readFloat();
    istream.readFloat();

    vertices[i].normal[0]   = istream.readFloat();
    vertices[i].normal[1]   = istream.readFloat();
    vertices[i].normal[2]   = istream.readFloat();

    // char colour[4]
    istream.readChar();
    istream.readChar();
    istream.readChar();
    istream.readChar();
  }

  nIndices = lumps[QBSPLump::INDICES].length / int( sizeof( int ) );
  indices = new int[nIndices];

  istream.reset();
  istream.skip( lumps[QBSPLump::INDICES].offset );

  for( int i = 0; i < nIndices; ++i ) {
    indices[i] = istream.readInt();
  }

  nFaces = lumps[QBSPLump::FACES].length / int( sizeof( QBSPFace ) );
  faces = new Face[nFaces];

  istream.reset();
  istream.skip( lumps[QBSPLump::FACES].offset );

  for( int i = 0; i < nFaces; ++i ) {
    faces[i].texture = istream.readInt();

    // int effect
    istream.readInt();
    // int type
    istream.readInt();

    faces[i].firstVertex = istream.readInt();
    faces[i].nVertices   = istream.readInt();

    faces[i].firstIndex  = istream.readInt();
    faces[i].nIndices    = istream.readInt();

    // int lightmap
    istream.readInt();
    // int lightmapCorner[2]
    istream.readInt();
    istream.readInt();
    // int lightmapSize[2]
    istream.readInt();
    istream.readInt();
    // float lightmapPos[3]
    istream.readFloat();
    istream.readFloat();
    istream.readFloat();
    // float lightmapVecs[2][3]
    istream.readFloat();
    istream.readFloat();
    istream.readFloat();
    istream.readFloat();
    istream.readFloat();
    istream.readFloat();
    // float normal[3]
    istream.readFloat();
    istream.readFloat();
    istream.readFloat();
    // int size[2]
    istream.readInt();
    istream.readInt();

    for( int j = 0; j < faces[i].nVertices; ++j ) {
      const client::Vertex& vertex = vertices[ faces[i].firstVertex + j ];

      if( vertex.pos[0] < -maxDim || vertex.pos[0] > +maxDim ||
          vertex.pos[1] < -maxDim || vertex.pos[1] > +maxDim ||
          vertex.pos[2] < -maxDim || vertex.pos[2] > +maxDim )
      {
        faces[i].nIndices = 0;
        break;
      }
    }
  }
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

        log.printRaw( "x" );
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

  delete[] usedBrushSides;

  log.printEnd( " OK" );

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
  log.println( "Optimising BSP model {" );
  log.indent();

  // remove faces that lay out of boundaries
  for( int i = 0; i < nFaces; ) {
    hard_assert( faces[i].nVertices > 0 && faces[i].nIndices >= 0 );

    if( faces[i].nIndices != 0 ) {
      ++i;
      continue;
    }

    aRemove( faces, i, nFaces );
    --nFaces;
    log.print( "outside face removed " );

    // adjust face references
    for( int j = 0; j < nModels; ++j ) {
      if( i < modelFaces[j].firstFace ) {
        --modelFaces[j].firstFace;
      }
      else if( i < modelFaces[j].firstFace + modelFaces[j].nFaces ) {
        hard_assert( modelFaces[j].nFaces > 0 );

        --modelFaces[j].nFaces;
      }
    }
    log.printEnd();
  }

  log.unindent();
  log.println( "}" );
}

void BSP::check() const
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

      if( usedBrushes.get( index ) ) {
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
    if( !usedLeaves.get( i ) ) {
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

void BSP::saveMatrix()
{
  String path = "bsp/" + name + ".ozBSP";

  log.print( "Dumping BSP structure to '%s' ...", path.cstr() );

  Buffer buffer( 4 * 1024 * 1024 );
  OutputStream os = buffer.outputStream();

  os.writePoint3( mins );
  os.writePoint3( maxs );
  os.writeFloat( life );
  os.writeFloat( resistance );

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
    os.writeFloat( models[i].timeout );

    int openSample  = models[i].openSample;
    int closeSample = models[i].closeSample;
    int frictSample = models[i].frictSample;

    os.writeString( openSample  == -1 ? "" : library.sounds[openSample].name );
    os.writeString( closeSample == -1 ? "" : library.sounds[closeSample].name );
    os.writeString( frictSample == -1 ? "" : library.sounds[frictSample].name );
  }

  buffer.write( path, os.length() );

  log.printEnd( " OK" );
}

void BSP::saveClient()
{
  String path = "bsp/" + name + ".ozcBSP";

  int flags = 0;

  compiler.beginMesh();

  compiler.enable( CAP_UNIQUE );
  compiler.enable( CAP_CW );

  for( int i = 0; i < nModels + 1; ++i ) {
    compiler.component( i );

    for( int j = 0; j < modelFaces[i].nFaces; ++j ) {
      const Face& face = faces[ modelFaces[i].firstFace + j ];

      int texId = textures[face.texture].id;
      if( texId == -1 ) {
        throw Exception( "BSP has a visible face without texture" );
      }

      library.usedTextures.set( texId );

      if( textures[face.texture].type & QBSP_WATER_TYPE_BIT ) {
        compiler.material( GL_DIFFUSE, 0.75f );
        compiler.material( GL_SPECULAR, 0.5f );
        flags |= Mesh::ALPHA_BIT;
      }
      else if( textures[face.texture].flags & QBSP_GLASS_FLAG_BIT ) {
        compiler.material( GL_DIFFUSE, 0.25f );
        compiler.material( GL_SPECULAR, 2.0f );
        flags |= Mesh::ALPHA_BIT;
      }
      else {
        compiler.material( GL_DIFFUSE, 1.0f );
        compiler.material( GL_SPECULAR, 0.0f );
        flags |= Mesh::SOLID_BIT;
      }

      compiler.texture( library.textures[texId].name );
      compiler.begin( GL_TRIANGLES );

      for( int k = 0; k < face.nIndices; ++k ) {
        const Vertex& vertex = vertices[ face.firstVertex + indices[face.firstIndex + k] ];

        compiler.texCoord( vertex.texCoord );
        compiler.normal( vertex.normal );
        compiler.vertex( vertex.pos );
      }

      if( textures[face.texture].type & QBSP_WATER_TYPE_BIT ) {
        for( int k = face.nIndices - 1; k >= 0; --k ) {
          const Vertex& vertex = vertices[ face.firstVertex + indices[face.firstIndex + k] ];

          compiler.texCoord( vertex.texCoord );
          compiler.normal( vertex.normal[0], -vertex.normal[1], -vertex.normal[2] );
          compiler.vertex( vertex.pos );
        }
      }

      compiler.end();
    }
  }

  compiler.endMesh();

  Buffer buffer( 4 * 1024 * 1024 );
  OutputStream ostream = buffer.outputStream();

  ostream.writeInt( flags );

  MeshData mesh;
  compiler.getMeshData( &mesh );
  mesh.write( &ostream, false );

  log.print( "Dumping BSP model to '%s' ...", path.cstr() );

  buffer.write( path, ostream.length() );

  log.printEnd( " OK" );
}

BSP::BSP( const char* name_ ) : name( name_ )
{}

BSP::~BSP()
{
  log.print( "Freeing BSP '%s' ...", name.cstr() );

  delete[] textures;
  delete[] planes;
  delete[] nodes;
  delete[] leaves;
  delete[] leafBrushes;
  delete[] models;
  delete[] brushes;
  delete[] brushSides;
  delete[] modelFaces;
  delete[] vertices;
  delete[] indices;
  delete[] faces;

  textures    = null;
  planes      = null;
  nodes       = null;
  leaves      = null;
  leafBrushes = null;
  models      = null;
  brushes     = null;
  brushSides  = null;
  modelFaces  = null;
  vertices    = null;
  indices     = null;
  faces       = null;

  nTextures    = 0;
  nPlanes      = 0;
  nNodes       = 0;
  nLeaves      = 0;
  nLeafBrushes = 0;
  nModels      = 0;
  nBrushes     = 0;
  nBrushSides  = 0;
  nVertices    = 0;
  nIndices     = 0;
  nFaces       = 0;

  log.printEnd( " OK" );
}

void BSP::prebuild( const char* name_ )
{
  String name = name_;

  log.println( "Prebuilding BSP '%s' {", name_ );
  log.indent();

  BSP* bsp = new BSP( name );
  bsp->load();
  bsp->optimise();
  bsp->check();
  bsp->saveMatrix();
  bsp->saveClient();
  delete bsp;

  log.unindent();
  log.println( "}" );
}

}
}
