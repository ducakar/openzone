/*
 *  BSP.cpp
 *
 *  Data structure for Quake3 BSP level
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/BSP.hpp"

#include "matrix/Library.hpp"

namespace oz
{

  bool BSP::loadOZBSP( const char* path )
  {
    Buffer buffer;
    if( !buffer.read( path ) ) {
      return false;
    }

    InputStream is = buffer.inputStream();

    mins         = is.readPoint3();
    maxs         = is.readPoint3();
    life         = is.readFloat();
    resistance   = is.readFloat();

    nPlanes      = is.readInt();
    nNodes       = is.readInt();
    nLeaves      = is.readInt();
    nLeafBrushes = is.readInt();
    nBrushes     = is.readInt();
    nBrushSides  = is.readInt();
    nModels      = is.readInt();

    size_t size = 0;

    size += size_t( nPlanes )      * sizeof( Plane );
    size = Alloc::alignUp( size );
    size += size_t( nNodes )       * sizeof( Node );
    size = Alloc::alignUp( size );
    size += size_t( nLeaves )      * sizeof( Leaf );
    size = Alloc::alignUp( size );
    size += size_t( nLeafBrushes ) * sizeof( int );
    size = Alloc::alignUp( size );
    size += size_t( nBrushes )     * sizeof( Brush );
    size = Alloc::alignUp( size );
    size += size_t( nBrushSides )  * sizeof( int );
    size = Alloc::alignUp( size );
    size += size_t( nModels )      * sizeof( Model );

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
      nodes[i].back = is.readInt();
    }
    data += nNodes * int( sizeof( Node ) );

    data = Alloc::alignUp( data );

    leaves = new( data ) Leaf[nLeaves];
    for( int i = 0; i < nLeaves; ++i ) {
      leaves[i].firstBrush = is.readInt();
      leaves[i].nBrushes = is.readInt();
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
      brushes[i].nSides = is.readInt();
      brushes[i].material = is.readInt();
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
      models[i].timeout = is.readFloat();
    }

    return true;
  }

  void BSP::freeOZBSP()
  {
    log.print( "Freeing BSP structure '%s' ...", library.bsps[id].name.cstr() );

    if( planes != null ) {
      aDestruct( planes, nPlanes );
      aDestruct( nodes, nNodes );
      aDestruct( leaves, nLeaves );
      aDestruct( leafBrushes, nLeafBrushes );
      aDestruct( brushes, nBrushes );
      aDestruct( brushSides, nBrushSides );
      aDestruct( models, nModels );

      delete[] reinterpret_cast<char*>( planes );

      nPlanes      = 0;
      nNodes       = 0;
      nLeaves      = 0;
      nLeafBrushes = 0;
      nBrushes     = 0;
      nBrushSides  = 0;
      nModels      = 0;

      planes      = null;
      nodes       = null;
      leaves      = null;
      leafBrushes = null;
      brushes     = null;
      brushSides  = null;
      models      = null;
    }

    log.printEnd( " OK" );
  }

  BSP::BSP( int id_ ) :
      id( id_ ),
      nPlanes( 0 ), nNodes( 0 ), nLeaves( 0 ), nLeafBrushes( 0 ), nModels( 0 ),
      nBrushes( 0 ), nBrushSides( 0 ),
      planes( null ), nodes( null ), leaves( null ), leafBrushes( null ), models( null ),
      brushes( null ), brushSides( null )
  {
    log.print( "Loading OpenZone BSP structure '%s' ...", library.bsps[id].name.cstr() );

    if( !loadOZBSP( library.bsps[id].path ) ) {
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
