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
 * @file build/Mouse.cc
 */

#include "stable.hh"

#include "build/Mouse.hh"

#include "client/ui/Mouse.hh"

#include "build/Context.hh"

namespace oz
{
namespace build
{

void Mouse::build()
{
  log.println( "Prebuilding mouse cursors {" );
  log.indent();

  File::mkdir( "ui" );
  File::mkdir( "ui/cur" );

  for( int i = 0; i < ui::Mouse::CURSORS_MAX; ++i ) {
    PhysFile inFile( String::str( "ui/cur/%s.in", ui::Mouse::NAMES[i] ) );

    FILE* fs = fopen( inFile.realPath(), "r" );
    if( fs == null ) {
      throw Exception( "Failed to open cursor description '%s'", inFile.realPath().cstr() );
    }

    int size, hotspotX, hotspotY;
    char imgPath[32];

    int nMatches = fscanf( fs, "%3d %3d %3d %31s", &size, &hotspotX, &hotspotY, imgPath );
    if( nMatches != 4 ) {
      fclose( fs );
      throw Exception( "Invalid xcursor line" );
    }

    fclose( fs );

    uint texId = context.loadRawTexture( String::str( "ui/cur/%s", imgPath ), false,
                                         GL_LINEAR, GL_LINEAR );

    BufferStream os;

    os.writeInt( size );
    os.writeInt( hotspotX );
    os.writeInt( hotspotY );
    context.writeTexture( texId, &os );

    glDeleteTextures( 1, &texId );

    File( String::str( "ui/cur/%s.ozcCur", ui::Mouse::NAMES[i] ) ).write( &os );
  }

  DArray<PhysFile> files = PhysFile( "ui/cur" ).ls();

  foreach( file, files.iter() ) {
    String fileName = file->name();

    if( fileName.beginsWith( "README" ) || fileName.beginsWith( "COPYING" ) ) {
      log.print( "Copying '%s' ...", fileName.cstr() );

      if( !file->map() ) {
        throw Exception( "Failed to copy '%s'", file->realPath().cstr() );
      }

      InputStream is = file->inputStream();
      File destFile( "ui/cur/" + fileName );

      if( !destFile.write( &is ) ) {
        throw Exception( "Failed to copy '%s'", file->realPath().cstr() );
      }

      file->unmap();

      log.printEnd( " OK" );
    }
  }

  log.unindent();
  log.println( "}" );
}

}
}
