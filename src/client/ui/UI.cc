/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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
 * @file client/ui/UI.cc
 */

#include "stable.hh"

#include "client/ui/UI.hh"

#include "client/Camera.hh"
#include "client/Colours.hh"
#include "client/Context.hh"
#include "client/Shape.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{
namespace ui
{

UI ui;

UI::UI() : root( null ), loadingScreen( null ), buildMenu( null ), debugFrame( null )
{}

void UI::showLoadingScreen( bool doShow )
{
  root->focus( loadingScreen );
  loadingScreen->show( doShow );
}

void UI::update()
{
  if( mouse.doShow == isFreelook ) {
    isFreelook = !mouse.doShow;

    foreach( area, root->children.iter() ) {
      if( !( area->flags & Area::PINNED_BIT ) ) {
        area->show( mouse.doShow );
      }
    }
  }

  if( !isFreelook ) {
    root->passMouseEvents();
  }
  Area::update();
}

void UI::draw()
{
  tf.ortho( Area::uiWidth, Area::uiHeight );
  tf.camera = Mat44::ID;

  // set shaders
  shader.setAmbientLight( Vec4( 0.6f, 0.5f, 0.6f, 1.0f ) );
  shader.setCaelumLight( ~Vec3( 0.6f, -0.3f, -0.8f ), Vec4( 0.6f, 0.6f, 0.6f, 1.0f ) );

  for( int i = 0; i < library.shaders.length(); ++i ) {
    if( shader.isLoaded || i == shader.plain ) {
      shader.use( i );

      tf.applyCamera();
      shader.updateLights();

      glUniform1f( param.oz_Fog_start, 1000000.0f );
      glUniform1f( param.oz_Fog_end, 1000000.0f );
    }
  }

  shader.use( shader.plain );

  glClear( GL_DEPTH_BUFFER_BIT );
  glLineWidth( 1.0f / uiScale );

  glEnable( GL_BLEND );

  root->drawChildren();
  mouse.draw();

  glDisable( GL_BLEND );

  OZ_GL_CHECK_ERROR();
}

void UI::load()
{
  isFreelook = false;

  buildMenu  = showBuild ? new BuildMenu() : null;
  debugFrame = showDebug ? new DebugFrame() : null;

  if( showBuild ) {
    root->add( buildMenu );
  }
  if( showDebug ) {
    root->add( debugFrame );
  }

  root->focus( loadingScreen );
}

void UI::unload()
{
  if( debugFrame != null ) {
    root->remove( debugFrame );
    debugFrame = null;
  }
  if( buildMenu != null ) {
    root->remove( buildMenu );
    buildMenu = null;
  }
}

void UI::init()
{
  isFreelook = false;

  uiScale   = config.getSet( "ui.scale",     1.0f  );
  uiAspect  = config.getSet( "ui.aspect",    0.0f  );
  showBuild = config.getSet( "ui.showBuild", false );
  showDebug = config.getSet( "ui.showDebug", false );

  if( uiAspect == 0.0f ) {
    Area::uiWidth   = int( float( camera.width  ) * uiScale + 0.5f );
    Area::uiHeight  = int( float( camera.height ) * uiScale + 0.5f );
    Area::uiCentreX = Area::uiWidth  / 2;
    Area::uiCentreY = Area::uiHeight / 2;
  }
  else {
    Area::uiWidth   = int( float( camera.height ) * uiScale * uiAspect + 0.5f );
    Area::uiHeight  = int( float( camera.height ) * uiScale + 0.5f );
    Area::uiCentreX = Area::uiWidth  / 2;
    Area::uiCentreY = Area::uiHeight / 2;
  }

  mouse.init();
  mouse.load();
  font.init();

  root = new Area( Area::uiWidth, Area::uiHeight );
  loadingScreen = new LoadingArea();

  root->add( loadingScreen );
}

void UI::free()
{
  delete root;

  Area::updateAreas.clear();
  Area::updateAreas.dealloc();

  font.free();
  mouse.unload();
  mouse.free();
}

}
}
}
