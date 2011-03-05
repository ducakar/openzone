/*
 *  UI.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "ui/UI.hpp"

#include "client/Camera.hpp"
#include "client/Context.hpp"
#include "client/Shape.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  UI ui;

  void UI::showLoadingScreen( bool doShow )
  {
    root->focus( loadingScreen );
    loadingScreen->show( doShow );
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
    hard_assert( glGetError() == GL_NO_ERROR );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, root->width, 0.0, root->height, -100.0, 100.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    shape.bindVertexArray();

    glEnable( GL_BLEND );

    shader.use( Shader::UI );

    root->drawChildren();
    mouse.draw();

    glDisable( GL_BLEND );

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void UI::load()
  {
    mouse.load();

    hud = new HudArea();
    strategic = new StrategicArea();

    root->add( hud );
    root->add( strategic );
    root->add( new DebugFrame() );
    root->add( new BuildMenu() );
    root->add( new InventoryMenu() );

    root->focus( loadingScreen );
  }

  void UI::unload()
  {
    mouse.unload();
  }

  void UI::init()
  {
    mouse.init();

    if( !font.init() ) {
      throw Exception( "Failed to load font" );
    }

    root = new Area( camera.width, camera.height );
    loadingScreen = new LoadingArea();

    hud = null;
    strategic = null;

    root->add( loadingScreen );
  }

  void UI::free()
  {
    delete root;

    Area::updateAreas.clear();
    Area::updateAreas.dealloc();

    font.free();
    mouse.free();
  }

}
}
}
