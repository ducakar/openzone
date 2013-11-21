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
 * @file matrix/BSP.cc
 */

#include <matrix/BSP.hh>

#include <common/Lingua.hh>
#include <matrix/Liber.hh>
#include <matrix/Collider.hh>

namespace oz
{

void BSP::load()
{
  Log::print( "Loading BSP structure '%s' ...", name.cstr() );

  File file = "@bsp/" + name + ".ozBSP";
  InputStream is = file.inputStream( Endian::LITTLE );

  if( !is.isAvailable() ) {
    OZ_ERROR( "BSP file '%s' read failed", file.path().cstr() );
  }

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

  // groundOffset
  is.readFloat();

  nPlanes       = is.readInt();
  nNodes        = is.readInt();
  nLeaves       = is.readInt();
  nLeafBrushes  = is.readInt();
  nBrushes      = is.readInt();
  nBrushSides   = is.readInt();
  nEntities     = is.readInt();
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
  size += size_t( nEntities )     * sizeof( entities[0] );
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

  entities = new( data ) EntityClass[nEntities];
  for( int i = 0; i < nEntities; ++i ) {
    entities[i].mins        = is.readPoint();
    entities[i].maxs        = is.readPoint();

    entities[i].title       = lingua.get( is.readString() );
    entities[i].bsp         = this;
    entities[i].move        = is.readVec3();

    entities[i].firstBrush  = is.readInt();
    entities[i].nBrushes    = is.readInt();

    entities[i].type        = EntityClass::Type( is.readInt() );
    entities[i].margin      = is.readFloat();
    entities[i].timeout     = is.readFloat();
    entities[i].ratioInc    = is.readFloat();

    entities[i].target      = is.readInt();
    entities[i].key         = is.readInt();

    entities[i].openSound   = liber.soundIndex( is.readString() );
    entities[i].closeSound  = liber.soundIndex( is.readString() );
    entities[i].frictSound  = liber.soundIndex( is.readString() );

    entities[i].model       = liber.modelIndex( is.readString() );
    entities[i].modelTransf = is.readMat44();
  }
  data += nEntities * int( sizeof( entities[0] ) );

  data = Alloc::alignUp( data );

  boundObjects = new( data ) BoundObject[nBoundObjects];
  for( int i = 0; i < nBoundObjects; ++i ) {
    boundObjects[i].clazz   = liber.objClass( is.readString() );
    boundObjects[i].pos     = is.readPoint();
    boundObjects[i].heading = Heading( is.readInt() );
  }

  hard_assert( !is.isAvailable() );

  Log::printEnd( " OK" );
}

void BSP::unload()
{
  if( planes != nullptr ) {
    Log::print( "Unloading BSP structure '%s' ...", name.cstr() );

    delete[] reinterpret_cast<char*>( planes );

    planes        = nullptr;
    nodes         = nullptr;
    leaves        = nullptr;
    leafBrushes   = nullptr;
    brushes       = nullptr;
    brushSides    = nullptr;
    entities      = nullptr;
    boundObjects  = nullptr;

    nPlanes       = 0;
    nNodes        = 0;
    nLeaves       = 0;
    nLeafBrushes  = 0;
    nBrushes      = 0;
    nBrushSides   = 0;
    nEntities     = 0;
    nBoundObjects = 0;

    Log::printEnd( " OK" );
  }
}

BSP::BSP( const char* name_, int id_ ) :
  planes( nullptr ), nodes( nullptr ), leaves( nullptr ), leafBrushes( nullptr ),
  entities( nullptr ), brushes( nullptr ), brushSides( nullptr ), boundObjects( nullptr ),
  nPlanes( 0 ), nNodes( 0 ), nLeaves( 0 ), nLeafBrushes( 0 ), nEntities( 0 ), nBrushes( 0 ),
  nBrushSides( 0 ), nBoundObjects( 0 ),
  name( name_ ), id( id_ ), nUsers( 0 )
{
  File file = String::str( "@bsp/%s.ozBSP", name_ );

  if( file.type() != File::REGULAR ) {
    OZ_ERROR( "BSP file '%s' read failed", file.path().cstr() );
  }

  InputStream is   = file.inputStream( Endian::LITTLE );

  mins             = is.readPoint();
  maxs             = is.readPoint();

  title            = lingua.get( is.readString() );
  description      = lingua.get( is.readString() );

  life             = is.readFloat();
  resistance       = is.readFloat();

  String sFragPool = is.readString();
  fragPool         = sFragPool.isEmpty() ? nullptr : liber.fragPool( sFragPool );
  nFrags           = is.readInt();

  sounds.resize( is.readInt() );
  for( int i = 0; i < sounds.length(); ++i ) {
    sounds[i] = liber.soundIndex( is.readString() );
  }

  demolishSound = liber.soundIndex( is.readString() );
  groundOffset  = is.readFloat();
}

}
