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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/ui/UI.cpp
 */

#include "stable.hpp"

#include "client/ui/UI.hpp"

#include "client/Camera.hpp"
#include "client/Colours.hpp"
#include "client/Context.hpp"
#include "client/Shape.hpp"
#include "client/OpenGL.hpp"

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
  OZ_GL_CHECK_ERROR();

  tf.ortho( camera.uiWidth, camera.uiHeight );
  tf.camera = Mat44::ID;

  // set shaders
  shader.setAmbientLight( Vec4( 0.6f, 0.5f, 0.6f, 1.0f ) );
  shader.setCaelumLight( Vec3( 0.67f, -0.67f, -0.33f ), Vec4( 0.6f, 0.6f, 0.6f, 1.0f ) );

  for( int i = 0; i < library.shaders.length(); ++i ) {
    if( shader.isLoaded || i == shader.plain ) {
      shader.use( i );
      tf.applyCamera();

      shader.setAmbientLight( Vec4( 0.6f, 0.5f, 0.6f, 1.0f ) );
      shader.setCaelumLight( Vec3( 0.67f, -0.67f, -0.33f ), Vec4( 0.6f, 0.6f, 0.6f, 1.0f ) );
      shader.updateLights();

      glUniform1f( param.oz_Fog_start, 1000000.0f );
      glUniform1f( param.oz_Fog_end, 2000000.0f );
    }
  }

  glEnable( GL_BLEND );

  glClear( GL_DEPTH_BUFFER_BIT );

  shader.use( shader.plain );

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
  float uiScale = config.getSet( "ui.scale", 1.0f );

  glLineWidth( 1.0f / uiScale );

  mouse.init();
  mouse.load();

  if( !font.init() ) {
    throw Exception( "Failed to load font" );
  }

  isFreelook = false;

  showBuild = config.getSet( "ui.showBuild", false );
  showDebug = config.getSet( "ui.showDebug", false );

  root = new Area( camera.uiWidth, camera.uiHeight );
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
