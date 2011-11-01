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
 * @file build/Mouse.cpp
 */

#include "stable.hpp"

#include "build/Mouse.hpp"

#include "client/OpenGL.hpp"
#include "client/ui/Mouse.hpp"

#include "build/Context.hpp"

namespace oz
{
namespace build
{

void Mouse::build()
{
  log.println( "Prebuilding mouse cursors {" );
  log.indent();

  for( int i = 0; i < ui::Mouse::MAX; ++i ) {
    FILE* f = fopen( "ui/cur/" + String( ui::Mouse::NAMES[i] ) + ".in", "r" );
    if( f == null ) {
      throw Exception( "Cursor prebuilding failed" );
    }

    int size, hotspotX, hotspotY;
    char imgFile[32];
    fscanf( f, "%d %d %d %31s", &size, &hotspotX, &hotspotY, imgFile );
    fclose( f );

    uint texId = Context::loadRawTexture( "ui/cur/" + String( imgFile ),
                                          false, GL_LINEAR, GL_LINEAR );

    Buffer buffer( 4 * 1024 * 1024 );
    OutputStream os = buffer.outputStream();

    os.writeInt( size );
    os.writeInt( hotspotX );
    os.writeInt( hotspotY );
    Context::writeTexture( texId, &os );

    glDeleteTextures( 1, &texId );

    buffer.write( "ui/cur/" + String( ui::Mouse::NAMES[i] ) + ".ozcCur", os.length() );
  }

  log.unindent();
  log.println( "}" );
}

}
}
