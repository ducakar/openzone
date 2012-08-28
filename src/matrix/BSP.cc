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

void BSP::load()
{
  Log::print( "Loading BSP structure '%s' ...", name.cstr() );

  String sPath = "bsp/" + name + ".ozBSP";

  PFile file( sPath );
  if( !file.map() ) {
    OZ_ERROR( "BSP file mmap failed" );
  }

  InputStream is = file.inputStream();

  // bounds
  is.readPoint();
  is.readPoint();

  // title
  is.readString();
  // description
  is.readString();

  // life
  is.readFloat();
  // resistance
  is.readFloat();

  // fragPool
  is.readString();
  // nFrags
  is.readInt();

  // sound samples
  int nSounds = is.readInt();
  for( int i = 0; i < nSounds; ++i ) {
    is.readString();
  }
  // demolishSound
  is.readString();

  nPlanes       = is.readInt();
  nNodes        = is.readInt();
  nLeaves       = is.readInt();
  nLeafBrushes  = is.readInt();
  nBrushes      = is.readInt();
  nBrushSides   = is.readInt();
  nModels       = is.readInt();
  nBoundObjects = is.readInt();

  size_t size = 0;

  size += size_t( nPlanes )       * sizeof( planes[0] );
  size  = Alloc::alignUp( size );
  size += size_t( nNodes )        * sizeof( nodes[0] );
  size  = Alloc::alignUp( size );
  size += size_t( nLeaves )       * sizeof( leaves[0] );
  size  = Alloc::alignUp( size );
  size += size_t( nLeafBrushes )  * sizeof( leafBrushes[0] );
  size  = Alloc::alignUp( size );
  size += size_t( nBrushes )      * sizeof( brushes[0] );
  size  = Alloc::alignUp( size );
  size += size_t( nBrushSides )   * sizeof( brushSides[0] );
  size  = Alloc::alignUp( size );
  size += size_t( nModels )       * sizeof( models[0] );
  size  = Alloc::alignUp( size );
  size += size_t( nBoundObjects ) * sizeof( boundObjects[0] );

  char* data = new char[size];

  hard_assert( data == Alloc::alignUp( data ) );

  planes = new( data ) Plane[nPlanes];
  for( int i = 0; i < nPlanes; ++i ) {
    planes[i] = is.readPlane();
  }
  data += nPlanes * int( sizeof( planes[0] ) );

  data = Alloc::alignUp( data );

  nodes = new( data ) Node[nNodes];
  for( int i = 0; i < nNodes; ++i ) {
    nodes[i].plane = is.readInt();
    nodes[i].front = is.readInt();
    nodes[i].back  = is.readInt();
  }
  data += nNodes * int( sizeof( nodes[0] ) );

  data = Alloc::alignUp( data );

  leaves = new( data ) Leaf[nLeaves];
  for( int i = 0; i < nLeaves; ++i ) {
    leaves[i].firstBrush = is.readInt();
    leaves[i].nBrushes   = is.readInt();
  }
  data += nLeaves * int( sizeof( leaves[0] ) );

  data = Alloc::alignUp( data );

  leafBrushes = new( data ) int[nLeafBrushes];
  for( int i = 0; i < nLeafBrushes; ++i ) {
    leafBrushes[i] = is.readInt();
  }
  data += nLeafBrushes * int( sizeof( leafBrushes[0] ) );

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
  data += nBrushes * int( sizeof( brushes[0] ) );

  data = Alloc::alignUp( data );

  brushSides = new( data ) int[nBrushSides];
  for( int i = 0; i < nBrushSides; ++i ) {
    brushSides[i] = is.readInt();
  }
  data += nBrushSides * int( sizeof( brushSides[0] ) );

  data = Alloc::alignUp( data );

  models = new( data ) Model[nModels];
  for( int i = 0; i < nModels; ++i ) {
    models[i].mins        = is.readPoint();
    models[i].maxs        = is.readPoint();

    models[i].title       = lingua.get( is.readString() );
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
  data += nModels * int( sizeof( models[0] ) );

  data = Alloc::alignUp( data );

  boundObjects = new( data ) BoundObject[nBoundObjects];
  for( int i = 0; i < nBoundObjects; ++i ) {
    boundObjects[i].clazz   = library.objClass( is.readString() );
    boundObjects[i].pos     = is.readPoint();
    boundObjects[i].heading = Heading( is.readInt() );
  }

  hard_assert( !is.isAvailable() );

  file.unmap();

  Log::printEnd( " OK" );
}

void BSP::unload()
{
  if( planes != null ) {
    Log::print( "Unloading BSP structure '%s' ...", name.cstr() );

    delete[] reinterpret_cast<char*>( planes );

    nPlanes       = 0;
    nNodes        = 0;
    nLeaves       = 0;
    nLeafBrushes  = 0;
    nBrushes      = 0;
    nBrushSides   = 0;
    nModels       = 0;
    nBoundObjects = 0;

    planes        = null;
    nodes         = null;
    leaves        = null;
    leafBrushes   = null;
    brushes       = null;
    brushSides    = null;
    models        = null;
    boundObjects  = null;

    Log::printEnd( " OK" );
  }
}

void BSP::init( const char* name_, int id_ )
{
  PFile file( String::str( "bsp/%s.ozBSP", name_ ) );
  if( !file.map() ) {
    OZ_ERROR( "BSP file mmap failed" );
  }

  InputStream is = file.inputStream();

  mins          = is.readPoint();
  maxs          = is.readPoint();

  nPlanes       = 0;
  nNodes        = 0;
  nLeaves       = 0;
  nLeafBrushes  = 0;
  nBrushes      = 0;
  nBrushSides   = 0;
  nModels       = 0;
  nBoundObjects = 0;

  planes        = null;
  nodes         = null;
  leaves        = null;
  leafBrushes   = null;
  brushes       = null;
  brushSides    = null;
  models        = null;
  boundObjects  = null;

  name          = name_;
  title         = lingua.get( is.readString() );
  description   = lingua.get( is.readString() );

  life          = is.readFloat();
  resistance    = is.readFloat();

  String sFragPool = is.readString();
  fragPool = sFragPool.isEmpty() ? null : library.fragPool( sFragPool );
  nFrags   = is.readInt();

  sounds.resize( is.readInt() );
  for( int i = 0; i < sounds.length(); ++i ) {
    sounds[i] = library.soundIndex( is.readString() );
  }

  String sDemolishSound = is.readString();
  demolishSound = sDemolishSound.isEmpty() ? -1 : library.soundIndex( sDemolishSound );

  id     = id_;
  nUsers = 0;

  file.unmap();
}

}
}
