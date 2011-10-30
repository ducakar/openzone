/*
 *  Terra.cpp
 *
 *  Matrix structure for terrain
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Terra.hpp"

#include "matrix/Library.hpp"

namespace oz
{

  const float Terra::Quad::SIZE     = float( SIZEI );
  const float Terra::Quad::INV_SIZE = 1.0f / float( SIZEI );
  const float Terra::Quad::DIM      = SIZE / 2.0f;

  const float Terra::DIM            = Terra::Quad::DIM * QUADS;

  Terra::Terra() : id( -1 )
  {}

  void Terra::load( int id_ )
  {
    id = id_;

    const String& name = library.terras[id].name;
    const String& path = library.terras[id].path;

    log.print( "Loading terrain '%s' ...", name.cstr() );

    Buffer buffer;
    if( !buffer.read( path ) ) {
      log.printEnd( " Cannot read file" );
      throw Exception( "Failed to load terrain" );
    }

    InputStream is = buffer.inputStream();

    int max = is.readInt();
    if( max != VERTS ) {
      log.printEnd( " Invalid dimension %d, should be %d", max, VERTS );
      throw Exception( "Failed to load terrain" );
    }

    for( int x = 0; x < VERTS; ++x ) {
      for( int y = 0; y < VERTS; ++y ) {
        quads[x][y].vertex       = is.readPoint3();
        quads[x][y].triNormal[0] = is.readVec3();
        quads[x][y].triNormal[1] = is.readVec3();
      }
    }

    log.printEnd( " OK" );
  }

  void Terra::init()
  {
    for( int x = 0; x < VERTS; ++x ) {
      for( int y = 0; y < VERTS; ++y ) {
        quads[x][y].vertex.x = float( x * Quad::SIZEI ) - DIM;
        quads[x][y].vertex.y = float( y * Quad::SIZEI ) - DIM;
        quads[x][y].vertex.z = 0.0f;
        quads[x][y].triNormal[0] = Vec3::ZERO;
        quads[x][y].triNormal[1] = Vec3::ZERO;
      }
    }
  }

  void Terra::read( InputStream* istream )
  {
    String name = istream->readString();
    int id = library.terraIndex( name );

    load( id );
  }

  void Terra::write( OutputStream* ostream ) const
  {
    ostream->writeString( library.terras[id].name );
  }

}
