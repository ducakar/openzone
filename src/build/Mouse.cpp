/*
 *  Mouse.cpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
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
