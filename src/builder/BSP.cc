/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file builder/BSP.cc
 */

#include <builder/BSP.hh>

#include <common/Timer.hh>
#include <builder/Context.hh>

namespace oz
{
namespace builder
{

const float BSP::DEFAULT_SCALE      = 0.01f;
const float BSP::DEFAULT_LIFE       = 10000.0f;
const float BSP::DEFAULT_RESISTANCE = 400.0f;
const float BSP::DEFAULT_MARGIN     = 0.1f;

const float BSP::LIQUID_ALPHA       = 0.75f;
const float BSP::LIQUID_SPECULAR    = 0.50f;
const float BSP::GLASS_ALPHA        = 0.15f;
const float BSP::GLASS_SPECULAR     = 2.00f;

void BSP::load()
{
  File configFile = "@baseq3/maps/" + name + ".json";
  File bspFile    = "@baseq3/maps/" + name + ".bsp";

  JSON config;
  if( !config.load( configFile ) ) {
    OZ_ERROR( "BSP config loading failed" );
  }

  title = config["title"].get( name );
  description = config["description"].get( "" );

  float scale = config["scale"].get( DEFAULT_SCALE );

  life = config["life"].get( DEFAULT_LIFE );
  resistance = config["resistance"].get( DEFAULT_RESISTANCE );

  waterFogColour = config["waterFogColour"].get( Vec4( 0.00f, 0.05f, 0.20f, 1.00f ) );
  lavaFogColour  = config["lavaFogColour"].get( Vec4( 0.30f, 0.20f, 0.00f, 1.00f ) );

  if( life <= 0.0f || !Math::isFinite( life ) ) {
    OZ_ERROR( "%s: Invalid life value. Should be > 0 and finite. If you want infinite life rather"
              " set resistance to infinity (\"inf\" or \"INF\").", name.cstr() );
  }
  if( resistance < 0.0f ) {
    OZ_ERROR( "%s: Invalid resistance. Should be >= 0.", name.cstr() );
  }

  fragPool = config["fragPool"].get( "" );
  nFrags   = config["nFrags"].get( 0 );

  demolishSound = config["demolishSound"].get( "" );
  groundOffset  = config["groundOffset"].get( 0.0f );

  mins = Point( -Math::INF, -Math::INF, -Math::INF );
  maxs = Point( +Math::INF, +Math::INF, +Math::INF );

  if( Math::isNaN( scale ) ) {
    OZ_ERROR( "Invalid BSP config" );
  }

  if( bspFile.type() != File::REGULAR ) {
    OZ_ERROR( "BSP reading failed" );
  }

  InputStream is = bspFile.inputStream( Endian::LITTLE );

  char id[4];
  id[0] = is.readChar();
  id[1] = is.readChar();
  id[2] = is.readChar();
  id[3] = is.readChar();

  int version = is.readInt();

  if( id[0] != 'I' || id[1] != 'B' || id[2] != 'S' || id[3] != 'P' || version != 46 ) {
    OZ_ERROR( "Not a Quake 3 BSP format" );
  }

  DArray<QBSPLump> lumps( QBSPLump::MAX );
  for( int i = 0; i < QBSPLump::MAX; ++i ) {
    lumps[i].offset = is.readInt();
    lumps[i].length = is.readInt();
  }

  textures.resize( lumps[QBSPLump::TEXTURES].length / int( sizeof( QBSPTexture ) ) );

  is.rewind();
  is.forward( lumps[QBSPLump::TEXTURES].offset );

  for( int i = 0; i < textures.length(); ++i ) {
    textures[i].name  = is.forward( 64 );
    textures[i].flags = is.readInt();
    textures[i].type  = is.readInt();

    if( textures[i].name.equals( "noshader" ) ) {
      textures[i].name = "";
    }
    else if( textures[i].name.length() <= 9 ) {
      OZ_ERROR( "Invalid texture name '%s'", textures[i].name.cstr() );
    }
    else {
      textures[i].name = textures[i].name.substring( 9 );
    }

    if( textures[i].type & QBSP_SEA_TYPE_BIT ) {
      textures[i].name = "@sea:" + textures[i].name;
    }

    Log::println( "Texture '%s' flags %x type %x",
                  textures[i].name.cstr(),
                  textures[i].flags,
                  textures[i].type );
  }

  planes.resize( lumps[QBSPLump::PLANES].length / int( sizeof( QBSPPlane ) ) );

  is.rewind();
  is.forward( lumps[QBSPLump::PLANES].offset );

  for( int i = 0; i < planes.length(); ++i ) {
    planes[i].n.x = is.readFloat();
    planes[i].n.y = is.readFloat();
    planes[i].n.z = is.readFloat();
    planes[i].d   = is.readFloat() * scale;
  }

  nodes.resize( lumps[QBSPLump::NODES].length / int( sizeof( QBSPNode ) ) );

  is.rewind();
  is.forward( lumps[QBSPLump::NODES].offset );

  for( int i = 0; i < nodes.length(); ++i ) {
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

  leaves.resize( lumps[QBSPLump::LEAFS].length / int( sizeof( QBSPLeaf ) ) );

  is.rewind();
  is.forward( lumps[QBSPLump::LEAFS].offset );

  for( int i = 0; i < leaves.length(); ++i ) {
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

  leafBrushes.resize( lumps[QBSPLump::LEAFBRUSHES].length / int( sizeof( int ) ) );

  is.rewind();
  is.forward( lumps[QBSPLump::LEAFBRUSHES].offset );

  for( int i = 0; i < leafBrushes.length(); ++i ) {
    leafBrushes[i] = is.readInt();
  }

  int nModels = lumps[QBSPLump::MODELS].length / int( sizeof( QBSPModel ) );

  models.resize( nModels - 1 );
  modelFaces.resize( nModels );

  is.rewind();
  is.forward( lumps[QBSPLump::MODELS].offset );

  const JSON& modelsConfig = config["entities"];

  if( !modelsConfig.isNull() && modelsConfig.type() != JSON::ARRAY ) {
    OZ_ERROR( "'models' entry in '%s' is not an array", configFile.path().cstr() );
  }

  // skip model 0 (whole BSP)
  is.forward( int( sizeof( QBSPModel ) ) );

  for( int i = 0; i < models.length(); ++i ) {
    const JSON& entityConfig = modelsConfig[i];

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
    models[i].title      = entityConfig["title"].get( "" );
    models[i].move       = entityConfig["move"].get( Vec3::ZERO );

    static const EnumName ENTITY_MAP[] = {
      { EntityClass::STATIC,         "STATIC"         },
      { EntityClass::MANUAL_DOOR,    "MANUAL_DOOR"    },
      { EntityClass::AUTO_DOOR,      "AUTO_DOOR"      },
      { EntityClass::IGNORING_BLOCK, "IGNORING_BLOCK" },
      { EntityClass::CRUSHING_BLOCK, "CRUSHING_BLOCK" },
      { EntityClass::ELEVATOR,       "ELEVATOR"       }
    };
    static const EnumMap<EntityClass::Type> entityMap( ENTITY_MAP );

    models[i].type       = entityMap[ entityConfig["type"].get( "" ) ];
    models[i].margin     = entityConfig["margin"].get( DEFAULT_MARGIN );
    models[i].timeout    = entityConfig["timeout"].get( Math::INF );
    models[i].ratioInc   = Timer::TICK_TIME / entityConfig["slideTime"].get( 1.0f );

    models[i].target     = entityConfig["target"].get( -1 );
    models[i].key        = entityConfig["key"].get( 0 );

    models[i].openSound  = entityConfig["openSound"].get( "" );
    models[i].closeSound = entityConfig["closeSound"].get( "" );
    models[i].frictSound = entityConfig["frictSound"].get( "" );

    if( models[i].type == EntityClass::ELEVATOR &&
        ( models[i].move.x != 0.0f || models[i].move.y != 0.0f ) )
    {
      OZ_ERROR( "Elevator can only move vertically, but model[%d].move = (%g %g %g)",
                i, models[i].move.x, models[i].move.y, models[i].move.z );
    }

    const JSON& modelConfig = entityConfig["model"];

    if( !modelConfig.isNull() ) {
      models[i].modelName = modelConfig["name"].get( "" );
      if( models[i].modelName.isEmpty() ) {
        OZ_ERROR( "model[%d].name is empty", i );
      }

      Vec3 translation = modelConfig["translation"].get( Vec3::ZERO );
      Vec3 rotation    = modelConfig["rotation"].get( Vec3::ZERO );

      models[i].modelTransf = Mat44::translation( models[i].p() + translation - Point::ORIGIN );
      models[i].modelTransf.rotateY( Math::rad( rotation.y ) );
      models[i].modelTransf.rotateX( Math::rad( rotation.x ) );
      models[i].modelTransf.rotateZ( Math::rad( rotation.z ) );
    }
  }

  is.rewind();
  is.forward( lumps[QBSPLump::MODELS].offset );

  for( int i = 0; i < models.length() + 1; ++i ) {
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

  brushSides.resize( lumps[QBSPLump::BRUSHSIDES].length / int( sizeof( QBSPBrushSide ) ) );

  is.rewind();
  is.forward( lumps[QBSPLump::BRUSHSIDES].offset );

  for( int i = 0; i < brushSides.length(); ++i ) {
    brushSides[i] = is.readInt();

    // int texture
    is.readInt();
  }

  brushes.resize( lumps[QBSPLump::BRUSHES].length / int( sizeof( QBSPBrush ) ) );

  if( brushes.length() > oz::BSP::MAX_BRUSHES ) {
    OZ_ERROR( "Too many brushes %d, maximum is %d", brushes.length(), oz::BSP::MAX_BRUSHES );
  }

  is.rewind();
  is.forward( lumps[QBSPLump::BRUSHES].offset );

  for( int i = 0; i < brushes.length(); ++i ) {
    brushes[i].firstSide = is.readInt();
    brushes[i].nSides    = is.readInt();
    brushes[i].flags     = 0;

    int texture = is.readInt();

    if( textures[texture].flags & QBSP_NONSOLID_FLAG_BIT ) {
      if( textures[texture].flags & QBSP_LADDER_FLAG_BIT ) {
        brushes[i].flags |= Medium::LADDER_BIT;
      }
      else if( textures[texture].type & QBSP_AIR_TYPE_BIT ) {
        brushes[i].flags |= Medium::AIR_BIT;
      }
      else if( textures[texture].type & QBSP_WATER_TYPE_BIT ) {
        brushes[i].flags |= Medium::WATER_BIT;
      }
      else if( textures[texture].type & QBSP_LAVA_TYPE_BIT ) {
        brushes[i].flags |= Medium::LAVA_BIT;
      }
      else if( textures[texture].type & QBSP_SEA_TYPE_BIT ) {
        brushes[i].flags |= Medium::SEA_BIT;
      }
    }
    else {
      brushes[i].flags |= Material::STRUCT_BIT;

      if( textures[texture].flags & QBSP_SLICK_FLAG_BIT ) {
        brushes[i].flags |= Material::SLICK_BIT;
      }
    }
  }

  vertices.resize( lumps[QBSPLump::VERTICES].length / int( sizeof( QBSPVertex ) ) );

  is.rewind();
  is.forward( lumps[QBSPLump::VERTICES].offset );

  for( int i = 0; i < vertices.length(); ++i ) {
    vertices[i].pos[0]      = is.readFloat() * scale;
    vertices[i].pos[1]      = is.readFloat() * scale;
    vertices[i].pos[2]      = is.readFloat() * scale;

    vertices[i].texCoord[0] = is.readFloat();
    vertices[i].texCoord[1] = is.readFloat();

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

  indices.resize( lumps[QBSPLump::INDICES].length / int( sizeof( int ) ) );

  is.rewind();
  is.forward( lumps[QBSPLump::INDICES].offset );

  for( int i = 0; i < indices.length(); ++i ) {
    indices[i] = is.readInt();
  }

  faces.resize( lumps[QBSPLump::FACES].length / int( sizeof( QBSPFace ) ) );

  is.rewind();
  is.forward( lumps[QBSPLump::FACES].offset );

  for( int i = 0; i < faces.length(); ++i ) {
    faces[i].texture     = is.readInt();

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
  }

  const JSON& objectsConfig = config["boundObjects"];
  int nBoundObjects = objectsConfig.length();

  for( int i = 0; i < nBoundObjects; ++i ) {
    const JSON& objectConfig = objectsConfig[i];

    String clazz = objectConfig["class"].get( "" );

    if( !clazz.isEmpty() ) {
      BoundObject object;

      object.clazz = clazz;
      object.pos   = objectConfig["pos"].get( Point::ORIGIN );

      String sHeading = objectConfig["heading"].get( "" );
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
        OZ_ERROR( "Missing heading for a BSP bound object" );
      }
      else {
        OZ_ERROR( "Invalid object heading '%s'", sHeading.cstr() );
      }

      boundObjects.add( object );
    }
  }

  config.clear( true );
}

void BSP::optimise()
{
  Log::println( "Optimising BSP structure {" );
  Log::indent();

  // remove brushes that lay out of boundaries
  for( int i = 0; i < brushes.length(); ) {
    hard_assert( brushes[i].nSides >= 0 );

    if( brushes[i].nSides != 0 ) {
      ++i;
      continue;
    }

    brushes.erase( i );
    Log::print( "outside brush removed " );

    // adjust brush references (for leaves)
    for( int j = 0; j < leafBrushes.length(); ) {
      if( leafBrushes[j] < i ) {
        ++j;
      }
      else if( i < leafBrushes[j] ) {
        --leafBrushes[j];
        ++j;
      }
      else {
        leafBrushes.erase( j );
        Log::printRaw( "." );

        for( int k = 0; k < leaves.length(); ++k ) {
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
    for( int j = 0; j < models.length(); ++j ) {
      if( i < models[j].firstBrush ) {
        --models[j].firstBrush;
      }
      else if( i < models[j].firstBrush + models[j].nBrushes ) {
        hard_assert( models[j].nBrushes > 0 );

        --models[j].nBrushes;
      }
    }
    Log::printEnd();
  }

  // remove model brushes from the static tree (WTF Quake BSP puts them there?)
  Log::print( "removing model brush references " );

  for( int i = 0; i < models.length(); ++i ) {
    for( int j = 0; j < models[i].nBrushes; ++j ) {
      int brush = models[i].firstBrush + j;

      for( int k = 0; k < leafBrushes.length(); ) {
        if( leafBrushes[k] != brush ) {
          ++k;
          continue;
        }

        leafBrushes.erase( k );
        Log::printRaw( "." );

        // adjust leaf references
        for( int l = 0; l < leaves.length(); ++l ) {
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

  Log::printEnd( " OK" );

  // remove unreferenced leaves
  Log::print( "removing unreferenced and empty leaves " );

  for( int i = 0; i < leaves.length(); ) {
    bool isReferenced = false;

    for( int j = 0; j < nodes.length(); ++j ) {
      if( nodes[j].front == ~i || nodes[j].back == ~i ) {
        isReferenced = true;
        break;
      }
    }

    if( isReferenced && leaves[i].nBrushes != 0 ) {
      ++i;
      continue;
    }

    leaves.erase( i );
    Log::printRaw( "." );

    // update references and tag unnecessary nodes, will be removed in the next pass (index 0 is
    // invalid as the root cannot be referenced)
    for( int j = 0; j < nodes.length(); ++j ) {
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

  Log::printEnd( " OK" );

  // collapse unnecessary nodes
  Log::print( "collapsing nodes " );

  bool hasCollapsed;
  do {
    hasCollapsed = false;

    for( int i = 0; i < nodes.length(); ) {
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

        Log::printRaw( "x" );
      }
      else {
        // find parent
        int* parentsRef = nullptr;
        for( int j = 0; j < nodes.length(); ++j ) {
          if( nodes[j].front == i ) {
            hard_assert( parentsRef == nullptr );

            parentsRef = &nodes[j].front;
          }
          if( nodes[j].back == i ) {
            hard_assert( parentsRef == nullptr );

            parentsRef = &nodes[j].back;
          }
        }
        hard_assert( parentsRef != nullptr );

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

      nodes.erase( i );

      for( int j = 0; j < nodes.length(); ++j ) {
        hard_assert( nodes[j].front != i );
        hard_assert( nodes[j].back != i );
      }

      // shift nodes' references
      for( int j = 0; j < nodes.length(); ++j ) {
        if( nodes[j].front > i ) {
          --nodes[j].front;
        }
        if( nodes[j].back > i ) {
          --nodes[j].back;
        }
      }

      Log::printRaw( "." );
      hasCollapsed = true;
    }
  }
  while( hasCollapsed );

  Log::printEnd( " OK" );

  // remove unused brush sides
  Log::print( "removing unused brush sides " );

  List<bool> usedBrushSides;
  usedBrushSides.resize( brushSides.length() );
  aFill<bool>( usedBrushSides.begin(), usedBrushSides.length(), false );

  for( int i = 0; i < brushes.length(); ++i ) {
    for( int j = 0; j < brushes[i].nSides; ++j ) {
      usedBrushSides[ brushes[i].firstSide + j ] = true;
    }
  }

  for( int i = 0; i < brushSides.length(); ) {
    if( usedBrushSides[i] ) {
      ++i;
      continue;
    }

    brushSides.erase( i );
    usedBrushSides.erase( i );
    Log::printRaw( "." );

    for( int j = 0; j < brushes.length(); ++j ) {
      if( i < brushes[j].firstSide ) {
        --brushes[j].firstSide;
      }
      else if( i < brushes[j].firstSide + brushes[j].nSides ) {
        // removed brush side shouldn't be referenced by any brush
        hard_assert( false );
      }
    }
  }

  usedBrushSides.clear();
  usedBrushSides.deallocate();

  Log::printEnd( " OK" );

  // remove unused planes
  Log::print( "removing unused planes " );

  List<bool> usedPlanes;
  usedPlanes.resize( planes.length() );
  aFill<bool>( usedPlanes.begin(), planes.length(), false );

  for( int i = 0; i < nodes.length(); ++i ) {
    usedPlanes[ nodes[i].plane ] = true;
  }
  for( int i = 0; i < brushSides.length(); ++i ) {
    usedPlanes[ brushSides[i] ] = true;
  }

  for( int i = 0; i < planes.length(); ) {
    if( usedPlanes[i] ) {
      ++i;
      continue;
    }

    planes.erase( i );
    usedPlanes.erase( i );
    Log::printRaw( "." );

    // adjust plane references
    for( int j = 0; j < nodes.length(); ++j ) {
      hard_assert( nodes[j].plane != i );

      if( nodes[j].plane > i ) {
        --nodes[j].plane;
      }
    }
    for( int j = 0; j < brushSides.length(); ++j ) {
      hard_assert( brushSides[j] != i );

      if( brushSides[j] > i ) {
        --brushSides[j];
      }
    }
  }

  usedPlanes.clear();
  usedPlanes.deallocate();

  Log::printEnd( " OK" );

  // optimise bounds
  Log::print( "Fitting bounds: " );

  mins = Point( +Math::INF, +Math::INF, +Math::INF );
  maxs = Point( -Math::INF, -Math::INF, -Math::INF );

  for( int i = 0; i < brushSides.length(); ++i ) {
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

  mins -= 2.0f * EPSILON * Vec3::ONE;
  maxs += 2.0f * EPSILON * Vec3::ONE;

  Log::printEnd( "(%g %g %g) (%g %g %g)", mins.x, mins.y, mins.z, maxs.x, maxs.y, maxs.z );

  Log::unindent();
  Log::println( "}" );
  Log::println( "Optimising BSP model {" );
  Log::indent();

  // remove faces that lay out of boundaries
  for( int i = 0; i < faces.length(); ) {
    hard_assert( faces[i].nVertices > 0 && faces[i].nIndices >= 0 );

    if( faces[i].nIndices != 0 ) {
      ++i;
      continue;
    }

    faces.erase( i );
    Log::println( "outside face removed" );

    // adjust face references
    for( int j = 0; j < models.length() + 1; ++j ) {
      if( i < modelFaces[j].firstFace ) {
        --modelFaces[j].firstFace;
      }
      else if( i < modelFaces[j].firstFace + modelFaces[j].nFaces ) {
        hard_assert( modelFaces[j].nFaces > 0 );

        --modelFaces[j].nFaces;
      }
    }
  }

  Log::unindent();
  Log::println( "}" );
}

void BSP::check() const
{
  Log::print( "Integrity check ..." );

  Bitset usedNodes( nodes.length() );
  Bitset usedLeaves( leaves.length() );
  Bitset usedBrushes( brushes.length() );

  usedNodes.clearAll();
  usedLeaves.clearAll();
  usedBrushes.clearAll();

  for( int i = 0; i < nodes.length(); ++i ) {
    if( nodes[i].front < 0 ) {
      if( usedLeaves.get( ~nodes[i].front ) ) {
        OZ_ERROR( "BSP leaf %d referenced twice", ~nodes[i].front );
      }
      usedLeaves.set( ~nodes[i].front );
    }
    else if( nodes[i].front != 0 ) {
      if( usedNodes.get( nodes[i].front ) ) {
        OZ_ERROR( "BSP node %d referenced twice", nodes[i].front );
      }
      usedNodes.set( nodes[i].front );
    }
    else {
      OZ_ERROR( "BSP root node referenced" );
    }

    if( nodes[i].back < 0 ) {
      if( usedLeaves.get( ~nodes[i].back ) ) {
        OZ_ERROR( "BSP leaf %d referenced twice", ~nodes[i].back );
      }
      usedLeaves.set( ~nodes[i].back );
    }
    else if( nodes[i].back != 0 ) {
      if( usedNodes.get( nodes[i].back ) ) {
        OZ_ERROR( "BSP node %d referenced twice", nodes[i].back );
      }
      usedNodes.set( nodes[i].back );
    }
    else {
      OZ_ERROR( "BSP root node referenced" );
    }
  }

  for( int i = 0; i < models.length(); ++i ) {
    for( int j = 0; j < models[i].nBrushes; ++j ) {
      int index = models[i].firstBrush + j;

      if( usedBrushes.get( index ) ) {
        OZ_ERROR( "BSP brush %d referenced by two models", index );
      }
      usedBrushes.set( index );
    }
  }

  usedBrushes.clearAll();
  for( int i = 0; i < leaves.length(); ++i ) {
    for( int j = 0; j < leaves[i].nBrushes; ++j ) {
      int index = leafBrushes[ leaves[i].firstBrush + j ];

      usedBrushes.set( index );
    }
  }
  for( int i = 0; i < models.length(); ++i ) {
    for( int j = 0; j < models[i].nBrushes; ++j ) {
      int index = models[i].firstBrush + j;

      if( usedBrushes.get( index ) ) {
        OZ_ERROR( "BSP model brush %d referenced by static tree", index );
      }
      usedBrushes.set( index );
    }
  }

  if( usedNodes.get( 0 ) ) {
    OZ_ERROR( "BSP root node referenced" );
  }
  for( int i = 1; i < nodes.length(); ++i ) {
    if( !usedNodes.get( i ) ) {
      OZ_ERROR( "BSP node %d not referenced", i );
    }
  }
  for( int i = 0; i < leaves.length(); ++i ) {
    if( !usedLeaves.get( i ) ) {
      OZ_ERROR( "BSP leaf %d not referenced", i );
    }
  }
  for( int i = 0; i < brushes.length(); ++i ) {
    if( !usedBrushes.get( i ) ) {
      OZ_ERROR( "BSP brush %d not referenced", i );
    }
  }

  for( int i = 0; i < planes.length(); ++i ) {
    if( !Math::isFinite( planes[i].d ) ) {
      OZ_ERROR( "BSP has invalid plane %d", i );
    }
  }

  Log::printEnd( " OK" );

  Log::println( "Statistics {" );
  Log::indent();
  Log::println( "%4d  models",      models.length() );
  Log::println( "%4d  nodes",       nodes.length() );
  Log::println( "%4d  leaves",      leaves.length() );
  Log::println( "%4d  brushes",     brushes.length() );
  Log::println( "%4d  brush sides", brushSides.length() );
  Log::println( "%4d  planes",      planes.length() );
  Log::println( "%4d  faces",       faces.length() );
  Log::println( "%4d  textures",    textures.length() );
  Log::unindent();
  Log::println( "}" );
}

void BSP::saveMatrix()
{
  File destFile = "bsp/" + name + ".ozBSP";

  Log::print( "Writing BSP structure to '%s' ...", destFile.path().cstr() );

  Set<String> usedSounds;

  foreach( model, models.citer() ) {
    if( !model->openSound.isEmpty() ) {
      usedSounds.include( model->openSound );
    }
    if( !model->closeSound.isEmpty() ) {
      usedSounds.include( model->closeSound );
    }
    if( !model->frictSound.isEmpty() ) {
      usedSounds.include( model->frictSound );
    }
  }
  if( !demolishSound.isEmpty() ) {
    usedSounds.include( demolishSound );
  }

  OutputStream os( 0, Endian::LITTLE );

  os.writePoint( mins );
  os.writePoint( maxs );

  os.writeString( title );
  os.writeString( description );

  os.writeFloat( life );
  os.writeFloat( resistance );

  os.writeString( fragPool );
  os.writeInt( nFrags );

  os.writeInt( usedSounds.length() );
  foreach( sound, usedSounds.citer() ) {
    os.writeString( *sound );
  }
  usedSounds.clear();
  usedSounds.deallocate();

  os.writeString( demolishSound );
  os.writeFloat( groundOffset );

  os.writeInt( planes.length() );
  os.writeInt( nodes.length() );
  os.writeInt( leaves.length() );
  os.writeInt( leafBrushes.length() );
  os.writeInt( brushes.length() );
  os.writeInt( brushSides.length() );
  os.writeInt( models.length() );
  os.writeInt( boundObjects.length() );

  foreach( plane, planes.citer() ) {
    os.writePlane( *plane );
  }

  foreach( node, nodes.citer() ) {
    os.writeInt( node->plane );
    os.writeInt( node->front );
    os.writeInt( node->back );
  }

  foreach( leaf, leaves.citer() ) {
    os.writeInt( leaf->firstBrush );
    os.writeInt( leaf->nBrushes );
  }

  foreach( leafBrush, leafBrushes.citer() ) {
    os.writeInt( *leafBrush );
  }

  foreach( brush, brushes.citer() ) {
    os.writeInt( brush->firstSide );
    os.writeInt( brush->nSides );
    os.writeInt( brush->flags );
  }

  foreach( brushSide, brushSides.citer() ) {
    os.writeInt( *brushSide );
  }

  foreach( model, models.citer() ) {
    os.writePoint( model->mins );
    os.writePoint( model->maxs );

    os.writeString( model->title );
    os.writeVec3( model->move );

    os.writeInt( model->firstBrush );
    os.writeInt( model->nBrushes );

    os.writeInt( int( model->type ) );
    os.writeFloat( model->margin );
    os.writeFloat( model->timeout );
    os.writeFloat( model->ratioInc );

    os.writeInt( model->target );
    os.writeInt( model->key );

    context.usedSounds.include( model->openSound, name + " (BSP)" );
    context.usedSounds.include( model->closeSound, name + " (BSP)" );
    context.usedSounds.include( model->frictSound, name + " (BSP)" );

    os.writeString( model->openSound );
    os.writeString( model->closeSound );
    os.writeString( model->frictSound );

    context.usedModels.include( model->modelName, name + " (BSP)" );

    os.writeString( model->modelName );
    os.writeMat44( model->modelTransf );
  }

  foreach( boundObject, boundObjects.citer() ) {
    os.writeString( boundObject->clazz );
    os.writePoint( boundObject->pos );
    os.writeInt( boundObject->heading );
  }

  if( !destFile.write( os.begin(), os.tell() ) ) {
    OZ_ERROR( "Failed to write '%s'", destFile.path().cstr() );
  }

  Log::printEnd( " OK" );
}

void BSP::saveClient()
{
  File destFile = "bsp/" + name + ".ozcModel";

  compiler.beginModel();

  compiler.enable( Compiler::UNIQUE );
  compiler.enable( Compiler::CLOCKWISE );

  for( int i = 0; i < models.length() + 1; ++i ) {
    compiler.beginNode();

    int lastTexture = faces[ modelFaces[i].firstFace ].texture;

    for( int j = 0; j < modelFaces[i].nFaces; ++j ) {
      const Face&    face = faces[ modelFaces[i].firstFace + j ];
      const Texture& tex  = textures[face.texture];

      if( tex.name.isEmpty() ) {
        OZ_ERROR( "BSP has a visible face without a texture" );
      }
      if( !tex.name.beginsWith( "@sea:" ) ) {
        context.usedTextures.include( tex.name, name + " (BSP)" );
      }

      if( face.texture != lastTexture ) {
        int meshId = compiler.endMesh();

        compiler.beginNode();
        compiler.bindMesh( meshId );
        compiler.endNode();
      }
      if( face.texture != lastTexture || j == 0 ) {
        lastTexture = face.texture;

        compiler.beginMesh();
        compiler.texture( tex.name );
        compiler.blend( tex.type & QBSP_ALPHA_TYPE_BIT );
      }

      compiler.begin( Compiler::TRIANGLES );

      for( int k = 0; k < face.nIndices; ++k ) {
        const client::Vertex& v = vertices[ face.firstVertex + indices[face.firstIndex + k] ];

        compiler.texCoord( v.texCoord );
        compiler.normal( v.normal );
        compiler.vertex( v.pos );
      }

      if( textures[face.texture].type & QBSP_WATER_TYPE_BIT ) {
        for( int k = face.nIndices - 1; k >= 0; --k ) {
          const client::Vertex& v = vertices[ face.firstVertex + indices[face.firstIndex + k] ];

          compiler.texCoord( v.texCoord );
          compiler.normal( -v.normal[0], -v.normal[1], -v.normal[2] );
          compiler.vertex( v.pos );
        }
      }

      compiler.end();
    }

    int meshId = compiler.endMesh();

    compiler.beginNode();
    compiler.bindMesh( meshId );
    compiler.endNode();

    compiler.endNode();
  }

  compiler.endModel();

  OutputStream os( 0, Endian::LITTLE );

  compiler.writeModel( &os, true );
  os.writeVec4( waterFogColour );
  os.writeVec4( lavaFogColour );

  Log::print( "Writing BSP model to '%s' ...", destFile.path().cstr() );

  if( !destFile.write( os.begin(), os.tell() ) ) {
    OZ_ERROR( "Failed to write '%s'", destFile.path().cstr() );
  }

  Log::printEnd( " OK" );
}

void BSP::build( const char* name_ )
{
  Log::println( "Prebuilding BSP '%s' {", name_ );
  Log::indent();

  name = name_;

  load();
  optimise();
  check();
  saveMatrix();
  saveClient();

  name          = "";
  title         = "";
  description   = "";
  fragPool      = "";
  demolishSound = "";

  textures.clear();
  planes.clear();
  planes.deallocate();
  nodes.clear();
  nodes.deallocate();
  leaves.clear();
  leaves.deallocate();
  leafBrushes.clear();
  leafBrushes.deallocate();
  models.clear();
  brushes.clear();
  brushes.deallocate();
  brushSides.clear();
  brushSides.deallocate();
  modelFaces.clear();
  vertices.clear();
  indices.clear();
  faces.clear();
  faces.deallocate();
  boundObjects.clear();
  boundObjects.deallocate();

  Log::unindent();
  Log::println( "}" );
}

BSP bsp;

}
}
