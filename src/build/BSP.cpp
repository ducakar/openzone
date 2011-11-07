/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file build/BSP.cpp
 */

#include "stable.hpp"

#include "build/BSP.hpp"

#include "matrix/Collider.hpp"
#include "matrix/BSP.hpp"
#include "matrix/Library.hpp"

#include "client/OpenGL.hpp"

#include "build/Compiler.hpp"

namespace oz
{
namespace build
{

const float BSP::DEFAULT_SCALE      = 0.01f;
const float BSP::DEFAULT_LIFE       = 10000.0f;
const float BSP::DEFAULT_RESISTANCE = 400.0f;

Bitset BSP::usedTextures;

inline bool BSP::includes( const matrix::BSP::Brush& brush, float maxDim ) const
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

  title = bspConfig.get( "title", name );
  description = bspConfig.get( "description", "" );

  float scale = bspConfig.get( "scale", DEFAULT_SCALE );
  float maxDim = bspConfig.get( "maxDim", Math::INF );

  life = bspConfig.get( "life", DEFAULT_LIFE );
  resistance = bspConfig.get( "resistance", DEFAULT_RESISTANCE );

  mins = Point3( -maxDim, -maxDim, -maxDim );
  maxs = Point3( +maxDim, +maxDim, +maxDim );

  if( Math::isNaN( scale ) || Math::isNaN( maxDim ) ) {
    throw Exception( "Invalid BSP config" );
  }

  File file( bspFile );
  if( !file.map() ) {
    throw Exception( "BSP reading failed" );
  }

  InputStream istream = file.inputStream();

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
  istream.forward( lumps[QBSPLump::TEXTURES].offset );

  for( int i = 0; i < nTextures; ++i ) {
    String name = istream.forward( 64 );

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
  istream.forward( lumps[QBSPLump::PLANES].offset );

  for( int i = 0; i < nPlanes; ++i ) {
    planes[i].nx = istream.readFloat();
    planes[i].ny = istream.readFloat();
    planes[i].nz = istream.readFloat();
    planes[i].d  = istream.readFloat() * scale;
  }

  nNodes = lumps[QBSPLump::NODES].length / int( sizeof( QBSPNode ) );
  nodes = new matrix::BSP::Node[nNodes];

  istream.reset();
  istream.forward( lumps[QBSPLump::NODES].offset );

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
  leaves = new matrix::BSP::Leaf[nLeaves];

  istream.reset();
  istream.forward( lumps[QBSPLump::LEAFS].offset );

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
  istream.forward( lumps[QBSPLump::LEAFBRUSHES].offset );

  for( int i = 0; i < nLeafBrushes; ++i ) {
    leafBrushes[i] = istream.readInt();
  }

  nModels = lumps[QBSPLump::MODELS].length / int( sizeof( QBSPModel ) ) - 1;
  models = null;
  modelFaces = new ModelFaces[nModels + 1];

  if( nModels != 0 ) {
    models = new matrix::BSP::Model[nModels];

    istream.reset();
    istream.forward( lumps[QBSPLump::MODELS].offset );

    hard_assert( nModels <= 99 );
    char keyBuffer[] = "model  ";

    // skip model 0 (whole BSP)
    istream.forward( int( sizeof( QBSPModel ) ) );

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
        models[i].type = matrix::BSP::Model::IGNORING;
      }
      else if( type.equals( "CRUSHING" ) ) {
        models[i].type = matrix::BSP::Model::CRUSHING;
      }
      else if( type.equals( "AUTO_DOOR" ) ) {
        models[i].type = matrix::BSP::Model::AUTO_DOOR;
      }
      else {
        throw Exception( "Invalid BSP model type, must be either IGNORING, CRUSHING or AUTO_DOOR" );
      }

      models[i].margin  = bspConfig.get( keyName + ".margin", 1.0f );
      models[i].timeout = bspConfig.get( keyName + ".timeout", 6.0f );

      String sOpenSound  = bspConfig.get( keyName + ".openSound", "" );
      String sCloseSound = bspConfig.get( keyName + ".closeSound", "" );
      String sFrictSound = bspConfig.get( keyName + ".frictSound", "" );

      models[i].openSound  = sOpenSound.isEmpty()  ? -1 : library.soundIndex( sOpenSound );
      models[i].closeSound = sCloseSound.isEmpty() ? -1 : library.soundIndex( sCloseSound );
      models[i].frictSound = sFrictSound.isEmpty() ? -1 : library.soundIndex( sFrictSound );
    }
  }

  istream.reset();
  istream.forward( lumps[QBSPLump::MODELS].offset );

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
  istream.forward( lumps[QBSPLump::BRUSHSIDES].offset );

  for( int i = 0; i < nBrushSides; ++i ) {
    brushSides[i] = istream.readInt();

    // int texture
    istream.readInt();
  }

  nBrushes = lumps[QBSPLump::BRUSHES].length / int( sizeof( QBSPBrush ) );
  brushes = new matrix::BSP::Brush[nBrushes];

  if( nBrushes > matrix::BSP::MAX_BRUSHES ) {
    throw Exception( "Too many brushes %d, can be at most %d", nBrushes, matrix::BSP::MAX_BRUSHES );
  }

  istream.reset();
  istream.forward( lumps[QBSPLump::BRUSHES].offset );

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
  vertices = new Vertex[nVertices];

  istream.reset();
  istream.forward( lumps[QBSPLump::VERTICES].offset );

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
  istream.forward( lumps[QBSPLump::INDICES].offset );

  for( int i = 0; i < nIndices; ++i ) {
    indices[i] = istream.readInt();
  }

  nFaces = lumps[QBSPLump::FACES].length / int( sizeof( QBSPFace ) );
  faces = new Face[nFaces];

  istream.reset();
  istream.forward( lumps[QBSPLump::FACES].offset );

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
      const Vertex& vertex = vertices[ faces[i].firstVertex + j ];

      if( vertex.pos[0] < -maxDim || vertex.pos[0] > +maxDim ||
          vertex.pos[1] < -maxDim || vertex.pos[1] > +maxDim ||
          vertex.pos[2] < -maxDim || vertex.pos[2] > +maxDim )
      {
        faces[i].nIndices = 0;
        break;
      }
    }
  }

  char keyBuffer[] = "object  ";
  for( int i = 0; i < BOUND_OBJECTS; ++i ) {
    hard_assert( i < 100 );

    keyBuffer[ sizeof( keyBuffer ) - 3 ] = char( '0' + ( i / 10 ) );
    keyBuffer[ sizeof( keyBuffer ) - 2 ] = char( '0' + ( i % 10 ) );

    String key     = keyBuffer;
    String objName = bspConfig.get( key + ".name", "" );

    if( !objName.isEmpty() ) {
      matrix::BSP::BoundObject object;

      object.clazz   = library.objClass( objName );

      object.pos.x   = bspConfig.get( key + ".pos.x", 0.0f );
      object.pos.y   = bspConfig.get( key + ".pos.y", 0.0f );
      object.pos.z   = bspConfig.get( key + ".pos.z", 0.0f );

      String sHeading = bspConfig.get( key + ".heading", "" );
      if( sHeading.equals( "NORTH" ) ) {
        object.heading = NORTH;
      }
      else if( sHeading.equals( "WEST" ) ) {
        object.heading = WEST;
      }
      else if( sHeading.equals( "SOUTH" ) ) {
        object.heading = SOUTH;
      }
      else if( sHeading.equals( "EAST" ) ) {
        object.heading = EAST;
      }
      else if( sHeading.isEmpty() ) {
        throw Exception( "Missing heading for a BSP bound object" );
      }
      else {
        throw Exception( "Invalid object heading '%s'", sHeading.cstr() );
      }

      boundObjects.add( object );
    }
  }

  file.unmap();
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
    for( int j = 0; j < nModels + 1; ++j ) {
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
        throw Exception( "BSP leaf %d referenced twice", ~nodes[i].front );
      }
      usedLeaves.set( ~nodes[i].front );
    }
    else if( nodes[i].front != 0 ) {
      if( usedNodes.get( nodes[i].front ) ) {
        throw Exception( "BSP node %d referenced twice", nodes[i].front );
      }
      usedNodes.set( nodes[i].front );
    }
    else {
      throw Exception( "BSP root node referenced" );
    }

    if( nodes[i].back < 0 ) {
      if( usedLeaves.get( ~nodes[i].back ) ) {
        throw Exception( "BSP leaf %d referenced twice", ~nodes[i].back );
      }
      usedLeaves.set( ~nodes[i].back );
    }
    else if( nodes[i].back != 0 ) {
      if( usedNodes.get( nodes[i].back ) ) {
        throw Exception( "BSP node %d referenced twice", nodes[i].back );
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
        throw Exception( "BSP brush %d referenced by two models", index );
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
        throw Exception( "BSP model brush %d referenced by static tree", index );
      }
      usedBrushes.set( index );
    }
  }

  if( usedNodes.get( 0 ) ) {
    throw Exception( "BSP root node referenced" );
  }
  for( int i = 1; i < nNodes; ++i ) {
    if( !usedNodes.get( i ) ) {
      throw Exception( "BSP node %d not referenced", i );
    }
  }
  for( int i = 0; i < nLeaves; ++i ) {
    if( !usedLeaves.get( i ) ) {
      throw Exception( "BSP leaf %d not referenced", i );
    }
  }
  for( int i = 0; i < nBrushes; ++i ) {
    if( !usedBrushes.get( i ) ) {
      throw Exception( "BSP brush %d not referenced", i );
    }
  }

  for( int i = 0; i < nPlanes; ++i ) {
    if( !Math::isFinite( planes[i].d ) ) {
      throw Exception( "BSP has invalid plane %d", i );
    }
  }

  log.printEnd( " OK" );
}

void BSP::saveMatrix()
{
  String path = "bsp/" + name + ".ozBSP";

  log.print( "Dumping BSP structure to '%s' ...", path.cstr() );

  Vector<int> sounds;
  for( int i = 0; i < nModels; ++i ) {
    sounds.include( models[i].openSound );
    sounds.include( models[i].closeSound );
    sounds.include( models[i].frictSound );
  }
  sounds.sort();

  if( !sounds.isEmpty() && sounds[0] == -1 ) {
    sounds.popFirst();
  }

  BufferStream os;

  os.writePoint3( mins );
  os.writePoint3( maxs );

  os.writeInt( sounds.length() );
  for( int i = 0; i < sounds.length(); ++i ) {
    os.writeString( library.sounds[ sounds[i] ].name );
  }

  os.writeString( title );
  os.writeString( description );

  sounds.clear();
  sounds.dealloc();

  os.writeFloat( life );
  os.writeFloat( resistance );

  os.writeInt( nPlanes );
  os.writeInt( nNodes );
  os.writeInt( nLeaves );
  os.writeInt( nLeafBrushes );
  os.writeInt( nBrushes );
  os.writeInt( nBrushSides );
  os.writeInt( nModels );
  os.writeInt( boundObjects.length() );

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

    int openSound  = models[i].openSound;
    int closeSound = models[i].closeSound;
    int frictSound = models[i].frictSound;

    os.writeString( openSound  == -1 ? "" : library.sounds[openSound].name );
    os.writeString( closeSound == -1 ? "" : library.sounds[closeSound].name );
    os.writeString( frictSound == -1 ? "" : library.sounds[frictSound].name );
  }

  for( int i = 0; i < boundObjects.length(); ++i ) {
    os.writeString( boundObjects[i].clazz->name );
    os.writePoint3( boundObjects[i].pos );
    os.writeInt( boundObjects[i].heading );
  }

  File( path ).write( &os );

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

      usedTextures.set( texId );

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
          compiler.normal( -vertex.normal[0], -vertex.normal[1], -vertex.normal[2] );
          compiler.vertex( vertex.pos );
        }
      }

      compiler.end();
    }
  }

  compiler.endMesh();

  BufferStream os;

  os.writeInt( flags );

  MeshData mesh;
  compiler.getMeshData( &mesh );
  mesh.write( &os, false );

  log.print( "Dumping BSP model to '%s' ...", path.cstr() );

  File( path ).write( &os );

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

void BSP::build( const char* name )
{
  log.println( "Prebuilding BSP '%s' {", name );
  log.indent();

  if( usedTextures.isEmpty() ) {
    usedTextures.alloc( library.textures.length() );
    usedTextures.clearAll();
  }

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
