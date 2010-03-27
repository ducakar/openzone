/*
 *  UI.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "stable.h"

#include "ui/UI.h"

#include "client/Camera.h"

#include <GL/glu.h>

namespace oz
{
namespace client
{
namespace ui
{

  UI ui;

  void UI::init()
  {
    mouse.init();

    if( !font.init() ) {
      throw Exception( "Failed to load font" );
    }

    root = new Area( camera.width, camera.height );
    hud = new HudArea();
    strategic = new StrategicArea();

    root->add( hud );
    root->add( strategic );
    root->add( new DebugFrame() );
    root->add( new BuildMenu() );
    root->add( new InventoryMenu() );
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
      root->passMouseEvents();
    }
    Area::update();
  }

  void UI::draw()
  {
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, root->width, 0.0, root->height, -100.0, 100.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    root->drawChildren();
    mouse.draw();
  }

}
}
}
