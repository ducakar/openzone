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
 * @file client/ui/Mouse.cc
 */

#include "stable.hh"

#include "client/ui/Mouse.hh"

#include "client/Context.hh"
#include "client/Shape.hh"
#include "client/OpenGL.hh"
#include "client/ui/Area.hh"

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

void Mouse::prepare()
{
  relX = 0;
  relY = 0;
  relZ = 0;

  oldButtons = buttons;
  buttons    = currButtons;
}

void Mouse::update()
{
#ifndef _WIN32
  if( isGrabOn ) {
    float move  = Math::sqrt( float( relX )*float( relX ) + float( relY )*float( relY ) );
    float accel = min( 1.0f + move * accelFactor, 2.0f );

    relX = int( float( relX ) * accel );
    relY = int( float( relY ) * accel );
  }
#endif

  if( doShow ) {
    icon = ARROW;

    int desiredX = x + relX;
    int desiredY = y + relY;

    x = clamp( desiredX, 0, Area::uiWidth  - 1 );
    y = clamp( desiredY, 0, Area::uiHeight - 1 );

    overEdgeX = x != desiredX ? desiredX - x : 0;
    overEdgeY = y != desiredY ? desiredY - y : 0;
  }
  else {
    x = Area::uiCentreX;
    y = Area::uiCentreY;

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

    File file( "ui/cur/" + String( NAMES[i] ) + ".ozcCur" );
    if( !file.map() ) {
      log.printEnd( " Failed" );
      throw Exception( "Cursor loading failed" );
    }

    InputStream is = file.inputStream();

    cursors[i].size     = is.readInt();
    cursors[i].hotspotX = is.readInt();
    cursors[i].hotspotY = is.readInt();
    cursors[i].texId    = context.readTexture( &is );

    file.unmap();

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
  doShow      = false;
  isGrabOn    = config.get( "screen.full", true );
  accelFactor = config.getSet( "mouse.accelFactor", 0.03125f );
  icon        = ARROW;

  x = Area::uiCentreX;
  y = Area::uiCentreY;
  relX = 0;
  relY = 0;
  relZ = 0;

  buttons     = 0;
  oldButtons  = 0;
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
