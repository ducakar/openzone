/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file build/Mouse.cc
 */

#include "stable.hh"

#include "build/Mouse.hh"

#include "client/OpenGL.hh"
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

  for( int i = 0; i < ui::Mouse::MAX; ++i ) {
    FILE* f = fopen( String::str( "ui/cur/%s.in", ui::Mouse::NAMES[i] ), "r" );
    if( f == null ) {
      throw Exception( "Cursor prebuilding failed" );
    }

    int size, hotspotX, hotspotY;
    char imgFile[32];
    fscanf( f, "%d %d %d %31s", &size, &hotspotX, &hotspotY, imgFile );
    fclose( f );

    uint texId = Context::loadRawTexture( String::str( "ui/cur/%s", imgFile ),
                                          false, GL_LINEAR, GL_LINEAR );

    BufferStream os;

    os.writeInt( size );
    os.writeInt( hotspotX );
    os.writeInt( hotspotY );
    Context::writeTexture( texId, &os );

    glDeleteTextures( 1, &texId );

    File( String::str( "ui/cur/%s.ozcCur", ui::Mouse::NAMES[i] ) ).write( &os );
  }

  log.unindent();
  log.println( "}" );
}

}
}
