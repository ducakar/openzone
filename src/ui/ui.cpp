/*
 *  ui.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "ui.h"

#include <GL/glu.h>

namespace oz
{
namespace client
{
namespace ui
{

  HudArea *hud;

  void init( int screenX, int screenY )
  {
    hud = new HudArea( screenX, screenY );
    mouse.init( screenX, screenY );

    if( !font.init() ) {
      throw Exception( "Failed to load font" );
    }
  }

  void free()
  {
    font.free();
    mouse.free();
  }

  void update()
  {
    if( mouse.doShow ) {
      hud->checkMouse();
    }
  }

  void draw()
  {
//    glEnable( GL_BLEND );
//    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, hud->width, 0.0, hud->height, 0.0, 1.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    hud->onDraw();
    mouse.draw();
  }

}
}
}
