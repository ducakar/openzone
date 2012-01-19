/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 */

/**
 * @file build/BSP.cc
 */

#include "stable.hh"

#include "build/BSP.hh"

#include "matrix/Collider.hh"
#include "matrix/BSP.hh"

#include "client/OpenGL.hh"

#include "build/Context.hh"
#include "build/Compiler.hh"

namespace oz
{
namespace build
{

const float BSP::DEFAULT_SCALE      = 0.01f;
const float BSP::DEFAULT_LIFE       = 10000.0f;
const float BSP::DEFAULT_RESISTANCE = 400.0f;

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
  PhysFile rcFile( String::str( "data/maps/%s.rc", name.cstr() ) );
  PhysFile bspFile( String::str( "data/maps/%s.bsp", name.cstr() ) );

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

  fragPool = bspConfig.get( "fragPool", "" );
  nFrags   = bspConfig.get( "nFrags", 0 );

  mins = Point3( -maxDim, -maxDim, -maxDim );
  maxs = Point3( +maxDim, +maxDim, +maxDim );

  if( Math::isnan( scale ) || Math::isnan( maxDim ) ) {
    throw Exception( "Invalid BSP config" );
  }

  if( !bspFile.map() ) {
    throw Exception( "BSP reading failed" );
  }

  InputStream is = bspFile.inputStream( Endian::LITTLE );

  char id[4];
  id[0] = is.readChar();
  id[1] = is.readChar();
  id[2] = is.readChar();
  id[3] = is.readChar();

  int version = is.readInt();

  if( id[0] != 'I' || id[1] != 'B' || id[2] != 'S' || id[3] != 'P' || version != 46 ) {
    throw Exception( "Wrong Quake 3 BSP format" );
  }

  DArray<QBSPLump> lumps( QBSPLump::MAX );
  for( int i = 0; i < QBSPLump::MAX; ++i ) {
    lumps[i].offset = is.readInt();
    lumps[i].length = is.readInt();
  }

  nTextures = lumps[QBSPLump::TEXTURES].length / int( sizeof( QBSPTexture ) );
  textures = new Texture[nTextures];

  is.reset();
  is.forward( lumps[QBSPLump::TEXTURES].offset );

  for( int i = 0; i < nTextures; ++i ) {
    textures[i].name  = is.forward( 64 );
    textures[i].flags = is.readInt();
    textures[i].type  = is.readInt();

    if( textures[i].name.length() < 10 || textures[i].name.endsWith( "NULL" ) ||
        ( textures[i].flags & QBSP_LADDER_FLAG_BIT ) )
    {
      textures[i].name = "";
    }
    else {
      textures[i].name = textures[i].name.substring( 9 );
    }

    log.println( "Texture '%s' flags %x type %x",
                 name.cstr(),
                 textures[i].flags,
                 textures[i].type );
  }

  nPlanes = lumps[QBSPLump::PLANES].length / int( sizeof( QBSPPlane ) );
  planes = new Plane[nPlanes];

  is.reset();
  is.forward( lumps[QBSPLump::PLANES].offset );

  for( int i = 0; i < nPlanes; ++i ) {
    planes[i].n.x = is.readFloat();
    planes[i].n.y = is.readFloat();
    planes[i].n.z = is.readFloat();
    planes[i].d   = is.readFloat() * scale;
  }

  nNodes = lumps[QBSPLump::NODES].length / int( sizeof( QBSPNode ) );
  nodes = new matrix::BSP::Node[nNodes];

  is.reset();
  is.forward( lumps[QBSPLump::NODES].offset );

  for( int i = 0; i < nNodes; ++i ) {
    nodes[i].plane = is.readInt();
    nodes[i].front = is.readInt();
    nodes[i].back  = is.readInt();

    // int bb[2][3]
    is.readInt();
    is.readInt();
    is.readInt();
    is.readInt();
    is.readInt();
    is.readInt();
  }

  nLeaves = lumps[QBSPLump::LEAFS].length / int( sizeof( QBSPLeaf ) );
  leaves = new matrix::BSP::Leaf[nLeaves];

  is.reset();
  is.forward( lumps[QBSPLump::LEAFS].offset );

  for( int i = 0; i < nLeaves; ++i ) {
    // int cluster
    is.readInt();
    // int area
    is.readInt();
    // int bb[2][3]
    is.readInt();
    is.readInt();
    is.readInt();
    is.readInt();
    is.readInt();
    is.readInt();
    // int firstFace
    is.readInt();
    // int nFaces
    is.readInt();

    leaves[i].firstBrush = is.readInt();
    leaves[i].nBrushes   = is.readInt();
  }

  nLeafBrushes = lumps[QBSPLump::LEAFBRUSHES].length / int( sizeof( int ) );
  leafBrushes = new int[nLeafBrushes];

  is.reset();
  is.forward( lumps[QBSPLump::LEAFBRUSHES].offset );

  for( int i = 0; i < nLeafBrushes; ++i ) {
    leafBrushes[i] = is.readInt();
  }

  nModels = lumps[QBSPLump::MODELS].length / int( sizeof( QBSPModel ) ) - 1;
  models = null;
  modelFaces = new ModelFaces[nModels + 1];

  if( nModels != 0 ) {
    models = new Model[nModels];

    is.reset();
    is.forward( lumps[QBSPLump::MODELS].offset );

    hard_assert( nModels < 100 );
    char keyBuffer[] = "model  ";

    // skip model 0 (whole BSP)
    is.forward( int( sizeof( QBSPModel ) ) );

    for( int i = 0; i < nModels; ++i ) {
      models[i].mins.x = is.readFloat() * scale - 4.0f * EPSILON;
      models[i].mins.y = is.readFloat() * scale - 4.0f * EPSILON;
      models[i].mins.z = is.readFloat() * scale - 4.0f * EPSILON;

      models[i].maxs.x = is.readFloat() * scale + 4.0f * EPSILON;
      models[i].maxs.y = is.readFloat() * scale + 4.0f * EPSILON;
      models[i].maxs.z = is.readFloat() * scale + 4.0f * EPSILON;

      // int firstFace
      is.readInt();
      // int nFaces
      is.readInt();

      models[i].firstBrush = is.readInt();
      models[i].nBrushes   = is.readInt();

      keyBuffer[5] = char( '0' + i / 10 );
      keyBuffer[6] = char( '0' + i % 10 );
      String keyName = keyBuffer;

      models[i].move.x = bspConfig.get( keyName + ".move.x", 0.0f );
      models[i].move.y = bspConfig.get( keyName + ".move.y", 0.0f );
      models[i].move.z = bspConfig.get( keyName + ".move.z", 0.0f );

      models[i].ratioInc = Timer::TICK_TIME / bspConfig.get( keyName + ".slideTime", 1.0f );
      models[i].flags    = 0;

      String sType = bspConfig.get( keyName + ".type", "IGNORING" );

      if( sType.equals( "IGNORING" ) ) {
        models[i].type = matrix::BSP::Model::IGNORING;
      }
      else if( sType.equals( "CRUSHING" ) ) {
        models[i].type = matrix::BSP::Model::CRUSHING;
      }
      else if( sType.equals( "AUTO_DOOR" ) ) {
        models[i].type = matrix::BSP::Model::AUTO_DOOR;
      }
      else if( sType.equals( "MANUAL_DOOR" ) ) {
        models[i].type = matrix::BSP::Model::MANUAL_DOOR;
      }
      else {
        throw Exception( "Invalid BSP model type, must be either IGNORING, CRUSHING, AUTO_DOOR "
                         "or MANUAL_DOOR." );
      }

      models[i].margin  = bspConfig.get( keyName + ".margin", 1.0f );
      models[i].timeout = bspConfig.get( keyName + ".timeout", 6.0f );

      models[i].openSound  = bspConfig.get( keyName + ".openSound", "" );
      models[i].closeSound = bspConfig.get( keyName + ".closeSound", "" );
      models[i].frictSound = bspConfig.get( keyName + ".frictSound", "" );
    }
  }

  is.reset();
  is.forward( lumps[QBSPLump::MODELS].offset );

  for( int i = 0; i < nModels + 1; ++i ) {
    // float bb[2][3]
    is.readFloat();
    is.readFloat();
    is.readFloat();
    is.readFloat();
    is.readFloat();
    is.readFloat();

    modelFaces[i].firstFace = is.readInt();
    modelFaces[i].nFaces    = is.readInt();

    // int firstBrush
    is.readInt();
    // int nBrushes
    is.readInt();
  }

  nBrushSides = lumps[QBSPLump::BRUSHSIDES].length / int( sizeof( QBSPBrushSide ) );
  brushSides = new int[nBrushSides];

  is.reset();
  is.forward( lumps[QBSPLump::BRUSHSIDES].offset );

  for( int i = 0; i < nBrushSides; ++i ) {
    brushSides[i] = is.readInt();

    // int texture
    is.readInt();
  }

  nBrushes = lumps[QBSPLump::BRUSHES].length / int( sizeof( QBSPBrush ) );
  brushes = new matrix::BSP::Brush[nBrushes];

  if( nBrushes > matrix::BSP::MAX_BRUSHES ) {
    throw Exception( "Too many brushes %d, can be at most %d", nBrushes, matrix::BSP::MAX_BRUSHES );
  }

  is.reset();
  is.forward( lumps[QBSPLump::BRUSHES].offset );

  for( int i = 0; i < nBrushes; ++i ) {
    brushes[i].firstSide = is.readInt();
    brushes[i].nSides    = is.readInt();
    brushes[i].material  = 0;

    int texture = is.readInt();

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

  is.reset();
  is.forward( lumps[QBSPLump::VERTICES].offset );

  for( int i = 0; i < nVertices; ++i ) {
    vertices[i].pos[0]      = is.readFloat() * scale;
    vertices[i].pos[1]      = is.readFloat() * scale;
    vertices[i].pos[2]      = is.readFloat() * scale;

    vertices[i].texCoord[0] = is.readFloat();
    vertices[i].texCoord[1] = 1.0f - is.readFloat();

    // float lightmapCoord[2]
    is.readFloat();
    is.readFloat();

    vertices[i].normal[0]   = is.readFloat();
    vertices[i].normal[1]   = is.readFloat();
    vertices[i].normal[2]   = is.readFloat();

    // char colour[4]
    is.readChar();
    is.readChar();
    is.readChar();
    is.readChar();
  }

  nIndices = lumps[QBSPLump::INDICES].length / int( sizeof( int ) );
  indices = new int[nIndices];

  is.reset();
  is.forward( lumps[QBSPLump::INDICES].offset );

  for( int i = 0; i < nIndices; ++i ) {
    indices[i] = is.readInt();
  }

  nFaces = lumps[QBSPLump::FACES].length / int( sizeof( QBSPFace ) );
  faces = new Face[nFaces];

  is.reset();
  is.forward( lumps[QBSPLump::FACES].offset );

  for( int i = 0; i < nFaces; ++i ) {
    faces[i].texture = is.readInt();

    // int effect
    is.readInt();
    // int type
    is.readInt();

    faces[i].firstVertex = is.readInt();
    faces[i].nVertices   = is.readInt();

    faces[i].firstIndex  = is.readInt();
    faces[i].nIndices    = is.readInt();

    // int lightmap
    is.readInt();
    // int lightmapCorner[2]
    is.readInt();
    is.readInt();
    // int lightmapSize[2]
    is.readInt();
    is.readInt();
    // float lightmapPos[3]
    is.readFloat();
    is.readFloat();
    is.readFloat();
    // float lightmapVecs[2][3]
    is.readFloat();
    is.readFloat();
    is.readFloat();
    is.readFloat();
    is.readFloat();
    is.readFloat();
    // float normal[3]
    is.readFloat();
    is.readFloat();
    is.readFloat();
    // int size[2]
    is.readInt();
    is.readInt();

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
      BoundObject object;

      object.clazz   = objName;
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

  bspFile.unmap();
  bspConfig.clear( true );
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

    if( plane.n.x == -1.0f ) {
      mins.x = min( -plane.d, mins.x );
    }
    else if( plane.n.x == 1.0f ) {
      maxs.x = max( +plane.d, maxs.x );
    }
    else if( plane.n.y == -1.0f ) {
      mins.y = min( -plane.d, mins.y );
    }
    else if( plane.n.y == 1.0f ) {
      maxs.y = max( +plane.d, maxs.y );
    }
    else if( plane.n.z == -1.0f ) {
      mins.z = min( -plane.d, mins.z );
    }
    else if( plane.n.z == 1.0f ) {
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
    if( !Math::isfinite( planes[i].d ) ) {
      throw Exception( "BSP has invalid plane %d", i );
    }
  }

  log.printEnd( " OK" );

  log.println( "Statistics {" );
  log.indent();
  log.println( "%4d  models",      nModels );
  log.println( "%4d  nodes",       nNodes );
  log.println( "%4d  leaves",      nLeaves );
  log.println( "%4d  brushes",     nBrushes );
  log.println( "%4d  brush sides", nBrushSides );
  log.println( "%4d  planes",      nPlanes );
  log.println( "%4d  faces",       nFaces );
  log.println( "%4d  textures",    nTextures );
  log.unindent();
  log.println( "}" );
}

void BSP::saveMatrix()
{
  File destFile( "bsp/" + name + ".ozBSP" );

  log.print( "Dumping BSP structure to '%s' ...", destFile.path().cstr() );

  Vector<String> sounds;

  for( int i = 0; i < nModels; ++i ) {
    if( !models[i].openSound.isEmpty() ) {
      sounds.include( models[i].openSound );
    }
    if( !models[i].closeSound.isEmpty() ) {
      sounds.include( models[i].closeSound );
    }
    if( !models[i].frictSound.isEmpty() ) {
      sounds.include( models[i].frictSound );
    }
  }
  sounds.sort();

  BufferStream os;

  os.writePoint3( mins );
  os.writePoint3( maxs );

  os.writeString( title );
  os.writeString( description );

  os.writeInt( sounds.length() );
  for( int i = 0; i < sounds.length(); ++i ) {
    os.writeString( sounds[i] );
  }

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

    context.usedSounds.include( models[i].openSound );
    context.usedSounds.include( models[i].closeSound );
    context.usedSounds.include( models[i].frictSound );

    os.writeString( models[i].openSound );
    os.writeString( models[i].closeSound );
    os.writeString( models[i].frictSound );

    os.writeString( models[i].keyClass );
  }

  for( int i = 0; i < boundObjects.length(); ++i ) {
    os.writeString( boundObjects[i].clazz );
    os.writePoint3( boundObjects[i].pos );
    os.writeInt( boundObjects[i].heading );
  }

  os.writeString( fragPool );
  os.writeInt( nFrags );

  if( !destFile.write( &os ) ) {
    throw Exception( "Failed to write '%s'", destFile.path().cstr() );
  }

  log.printEnd( " OK" );
}

void BSP::saveClient()
{
  File destFile( "bsp/" + name + ".ozcBSP" );

  int flags = 0;

  compiler.beginMesh();

  compiler.enable( CAP_UNIQUE );
  compiler.enable( CAP_CW );

  for( int i = 0; i < nModels + 1; ++i ) {
    compiler.component( i );

    for( int j = 0; j < modelFaces[i].nFaces; ++j ) {
      const Face& face = faces[ modelFaces[i].firstFace + j ];

      if( textures[face.texture].name.isEmpty() ) {
        throw Exception( "BSP has a visible face without texture" );
      }

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

      context.usedTextures.include( textures[face.texture].name );

      compiler.texture( textures[face.texture].name );
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

  log.print( "Dumping BSP model to '%s' ...", destFile.path().cstr() );

  if( !destFile.write( &os ) ) {
    throw Exception( "Failed to write '%s'", destFile.path().cstr() );
  }

  log.printEnd( " OK" );
}

BSP::BSP( const char* name_ ) :
  name( name_ )
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
