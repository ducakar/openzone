/*
 *  Mouse.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ui/Mouse.hpp"

#include "client/Camera.hpp"
#include "client/Context.hpp"
#include "client/Shape.hpp"

#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  Mouse mouse;

  const char* Mouse::NAMES[Mouse::MAX] = {
    "X_cursor",
    "left_ptr",
    "fleur",
    "xterm",
    "hand2"
  };

#ifdef OZ_TOOLS
  void Mouse::prebuild()
  {
    log.println( "Prebuilding mouse cursors {" );
    log.indent();

    for( int i = 0; i < MAX; ++i ) {
      FILE* f = fopen( "ui/cur/" + String( NAMES[i] ) + ".in", "r" );
      if( f == null ) {
        throw Exception( "Cursor prebuilding failed" );
      }

      int size, hotspotX, hotspotY;
      char imgFile[32];
      fscanf( f, "%d %d %d %31s", &size, &hotspotX, &hotspotY, imgFile );
      fclose( f );

      uint texId = context.loadRawTexture( "ui/cur/" + String( imgFile ),
                                           false, GL_LINEAR, GL_LINEAR );

      OutputStream os = buffer.outputStream();

      os.writeInt( size );
      os.writeInt( hotspotX );
      os.writeInt( hotspotY );
      context.writeTexture( texId, &os );

      glDeleteTextures( 1, &texId );

      buffer.write( "ui/cur/" + String( NAMES[i] ) + ".ozcCur", os.length() );
    }

    log.unindent();
    log.println( "}" );
  }
#endif

  void Mouse::prepare()
  {
    relX = 0;
    relY = 0;
    relZ = 0;

    oldButtons = buttons;
    buttons = currButtons;
  }

  void Mouse::update()
  {
    if( doShow ) {
      icon = ARROW;

      int desiredX = x + relX;
      int desiredY = y + relY;

      x = clamp( desiredX, 0, camera.width - 1 );
      y = clamp( desiredY, 0, camera.height - 1 );

      overEdgeX = x != desiredX ? desiredX - x : 0;
      overEdgeY = y != desiredY ? desiredY - y : 0;
    }
    else {
      x = camera.centreX;
      y = camera.centreY;

      overEdgeX = relX;
      overEdgeY = relY;
    }

    leftClick   = ( buttons & ~oldButtons & SDL_BUTTON_LMASK ) != 0;
    middleClick = ( buttons & ~oldButtons & SDL_BUTTON_MMASK ) != 0;
    rightClick  = ( buttons & ~oldButtons & SDL_BUTTON_RMASK ) != 0;
    wheelUp     = ( buttons & ~oldButtons & SDL_BUTTON_WUMASK ) != 0;
    wheelDown   = ( buttons & ~oldButtons & SDL_BUTTON_WDMASK ) != 0;
  }

  void Mouse::draw() const
  {
    const Cursor& cur = cursors[icon];

    if( doShow ) {
      glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );
      glBindTexture( GL_TEXTURE_2D, cur.texId );
      shape.fill( x - cur.hotspotX, y + 1 + cur.hotspotY - cur.size, cur.size, cur.size );
      glBindTexture( GL_TEXTURE_2D, 0 );
    }
  }

  void Mouse::load()
  {
    log.println( "Loading mouse cursors {" );
    log.indent();

    for( int i = 0; i < MAX; ++i ) {
      log.print( "Loading cursor '%s' ...", NAMES[i] );

      if( !buffer.read( "ui/cur/" + String( NAMES[i] ) + ".ozcCur" ) ) {
        log.printEnd( " Failed" );
        throw Exception( "Cursor loading failed" );
      }

      InputStream is = buffer.inputStream();

      cursors[i].size     = is.readInt();
      cursors[i].hotspotX = is.readInt();
      cursors[i].hotspotY = is.readInt();
      cursors[i].texId    = context.readTexture( &is );

      log.printEnd( " OK" );
    }

    log.unindent();
    log.println( "}" );
  }

  void Mouse::unload()
  {
    glDeleteTextures( 1, &cursors[X].texId );
    glDeleteTextures( 1, &cursors[ARROW].texId );
    glDeleteTextures( 1, &cursors[MOVE].texId );
    glDeleteTextures( 1, &cursors[TEXT].texId );

    cursors[X].texId     = 0;
    cursors[ARROW].texId = 0;
    cursors[MOVE].texId  = 0;
    cursors[TEXT].texId  = 0;
  }

  void Mouse::init()
  {
    icon = ARROW;
    doShow = false;

    x = camera.centreX;
    y = camera.centreY;
    relX = 0;
    relY = 0;

    buttons = 0;
    oldButtons = 0;
    currButtons = 0;

    cursors[X].texId     = 0;
    cursors[ARROW].texId = 0;
    cursors[MOVE].texId  = 0;
    cursors[TEXT].texId  = 0;
  }

  void Mouse::free()
  {}

}
}
}
