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
 * @file matrix/BSP.cc
 */

#include "stable.hh"

#include "matrix/BSP.hh"

#include "matrix/Library.hh"
#include "matrix/Collider.hh"

namespace oz
{
namespace matrix
{

BSP::BSP( const char* name_, int id_ ) :
  name( name_ ), id( id_ )
{
  life          = 0.0f;
  resistance    = 0.0f;

  nPlanes       = 0;
  nNodes        = 0;
  nLeaves       = 0;
  nLeafBrushes  = 0;
  nBrushes      = 0;
  nBrushSides   = 0;
  nModels       = 0;

  planes        = null;
  nodes         = null;
  leaves        = null;
  leafBrushes   = null;
  brushes       = null;
  brushSides    = null;
  models        = null;

  fragPool      = null;
  nFrags        = 0;

  demolishSound = -1;

  nUsers        = 0;
}

BSP::~BSP()
{
  hard_assert( planes == null );
}

void BSP::load()
{
  log.print( "Loading BSP structure '%s' ...", name.cstr() );

  String sPath = "bsp/" + name + ".ozBSP";

  PhysFile file( sPath );
  if( !file.map() ) {
    throw Exception( "BSP file mmap failed" );
  }

  InputStream is = file.inputStream();

  // bounds
  is.readPoint();
  is.readPoint();

  // title
  is.readString();
  // description
  is.readString();

  // sound samples
  is.readInt();
  for( int i = 0; i < sounds.length(); ++i ) {
    is.readString();
  }

  life          = is.readFloat();
  resistance    = is.readFloat();

  nPlanes       = is.readInt();
  nNodes        = is.readInt();
  nLeaves       = is.readInt();
  nLeafBrushes  = is.readInt();
  nBrushes      = is.readInt();
  nBrushSides   = is.readInt();
  nModels       = is.readInt();
  nBoundObjects = is.readInt();

  size_t size = 0;

  size += size_t( nPlanes )       * sizeof( Plane );
  size  = Alloc::alignUp( size );
  size += size_t( nNodes )        * sizeof( Node );
  size  = Alloc::alignUp( size );
  size += size_t( nLeaves )       * sizeof( Leaf );
  size  = Alloc::alignUp( size );
  size += size_t( nLeafBrushes )  * sizeof( int );
  size  = Alloc::alignUp( size );
  size += size_t( nBrushes )      * sizeof( Brush );
  size  = Alloc::alignUp( size );
  size += size_t( nBrushSides )   * sizeof( int );
  size  = Alloc::alignUp( size );
  size += size_t( nModels )       * sizeof( Model );
  size  = Alloc::alignUp( size );
  size += size_t( nBoundObjects ) * sizeof( BoundObject );

  char* data = new char[size];

  hard_assert( data == Alloc::alignUp( data ) );

  planes = new( data ) Plane[nPlanes];
  for( int i = 0; i < nPlanes; ++i ) {
    planes[i] = is.readPlane();
  }
  data += nPlanes * int( sizeof( Plane ) );

  data = Alloc::alignUp( data );

  nodes = new( data ) Node[nNodes];
  for( int i = 0; i < nNodes; ++i ) {
    nodes[i].plane = is.readInt();
    nodes[i].front = is.readInt();
    nodes[i].back  = is.readInt();
  }
  data += nNodes * int( sizeof( Node ) );

  data = Alloc::alignUp( data );

  leaves = new( data ) Leaf[nLeaves];
  for( int i = 0; i < nLeaves; ++i ) {
    leaves[i].firstBrush = is.readInt();
    leaves[i].nBrushes   = is.readInt();
  }
  data += nLeaves * int( sizeof( Leaf ) );

  data = Alloc::alignUp( data );

  leafBrushes = new( data ) int[nLeafBrushes];
  for( int i = 0; i < nLeafBrushes; ++i ) {
    leafBrushes[i] = is.readInt();
  }
  data += nLeafBrushes * int( sizeof( int ) );

  data = Alloc::alignUp( data );

  brushes = new( data ) Brush[nBrushes];
  for( int i = 0; i < nBrushes; ++i ) {
    brushes[i].firstSide = is.readInt();
    brushes[i].nSides    = is.readInt();
    brushes[i].flags     = is.readInt();

    if( brushes[i].flags & Medium::SEA_BIT ) {
      brushes[i].flags |= orbis.terra.liquid & Medium::LIQUID_MASK;
    }
  }
  data += nBrushes * int( sizeof( Brush ) );

  data = Alloc::alignUp( data );

  brushSides = new( data ) int[nBrushSides];
  for( int i = 0; i < nBrushSides; ++i ) {
    brushSides[i] = is.readInt();
  }
  data += nBrushSides * int( sizeof( int ) );

  data = Alloc::alignUp( data );

  models = new( data ) Model[nModels];
  for( int i = 0; i < nModels; ++i ) {
    models[i].mins        = is.readPoint();
    models[i].maxs        = is.readPoint();

    models[i].name        = is.readString();
    models[i].title       = lingua.get( models[i].name );
    models[i].bsp         = this;
    models[i].move        = is.readVec3();

    models[i].firstBrush  = is.readInt();
    models[i].nBrushes    = is.readInt();

    models[i].type        = Model::Type( is.readInt() );
    models[i].margin      = is.readFloat();
    models[i].timeout     = is.readFloat();
    models[i].ratioInc    = is.readFloat();

    models[i].target      = is.readInt();
    models[i].key         = is.readInt();

    String sOpenSound     = is.readString();
    String sCloseSound    = is.readString();
    String sFrictSound    = is.readString();

    models[i].openSound   = sOpenSound.isEmpty()  ? -1 : library.soundIndex( sOpenSound );
    models[i].closeSound  = sCloseSound.isEmpty() ? -1 : library.soundIndex( sCloseSound );
    models[i].frictSound  = sFrictSound.isEmpty() ? -1 : library.soundIndex( sFrictSound );
  }
  data += nModels * int( sizeof( Model ) );

  data = Alloc::alignUp( data );

  boundObjects = new( data ) BoundObject[nBoundObjects];
  for( int i = 0; i < nBoundObjects; ++i ) {
    boundObjects[i].clazz   = library.objClass( is.readString() );
    boundObjects[i].pos     = is.readPoint();
    boundObjects[i].heading = Heading( is.readInt() );
  }

  String sFragPool = is.readString();

  fragPool = sFragPool.isEmpty() ? null : library.fragPool( sFragPool );
  nFrags   = is.readInt();

  String sDemolishSound = is.readString();

  demolishSound = sDemolishSound.isEmpty() ? -1 : library.soundIndex( sDemolishSound );

  hard_assert( !is.isAvailable() );

  file.unmap();

  log.printEnd( " OK" );
}

void BSP::unload()
{
  log.print( "Unloading BSP structure '%s' ...", name.cstr() );

  if( planes != null ) {
    delete[] reinterpret_cast<char*>( planes );

    life         = 0.0f;
    resistance   = 0.0f;

    nPlanes      = 0;
    nNodes       = 0;
    nLeaves      = 0;
    nLeafBrushes = 0;
    nBrushes     = 0;
    nBrushSides  = 0;
    nModels      = 0;

    planes       = null;
    nodes        = null;
    leaves       = null;
    leafBrushes  = null;
    brushes      = null;
    brushSides   = null;
    models       = null;

    fragPool     = null;
    nFrags       = 0;

    nUsers       = 0;
  }

  log.printEnd( " OK" );
}

void BSP::init()
{
  String sPath = "bsp/" + name + ".ozBSP";

  PhysFile file( sPath );
  if( !file.map() ) {
    throw Exception( "BSP file mmap failed" );
  }

  InputStream is = file.inputStream();

  mins        = is.readPoint();
  maxs        = is.readPoint();

  title       = lingua.get( is.readString() );
  description = lingua.get( is.readString() );

  int nSounds = is.readInt();
  if( nSounds != 0 ) {
    sounds.alloc( nSounds );

    for( int i = 0; i < nSounds; ++i ) {
      sounds.add( library.soundIndex( is.readString() ) );
    }
  }

  file.unmap();
}

}
}
