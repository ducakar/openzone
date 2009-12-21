/*
 *  UI.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "UI.h"

#include <GL/glu.h>

namespace oz
{
namespace client
{
namespace ui
{

  UI ui;

  void UI::init( int screenX, int screenY )
  {
    mouse.init( screenX, screenY );

    if( !font.init() ) {
      throw Exception( "Failed to load font" );
    }

    hud = new HudArea( screenX, screenY );
  }

  void UI::free()
  {
    delete hud;

    font.free();
    mouse.free();
  }

  void UI::update()
  {
    if( mouse.doShow ) {
      hud->checkMouse();
    }
  }

  void UI::draw()
  {
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, hud->width, 0.0, hud->height, -100.0, 100.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    hud->onDraw();
    mouse.draw();
  }

}
}
}
