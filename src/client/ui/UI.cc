/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
#include "client/Context.hh"
#include "client/Shape.hh"
#include "client/OpenGL.hh"

#include "client/ui/LoadingArea.hh"
#include "client/ui/QuestFrame.hh"
#include "client/ui/GalileoFrame.hh"
#include "client/ui/MusicPlayer.hh"
#include "client/ui/BuildMenu.hh"
#include "client/ui/DebugFrame.hh"

namespace oz
{
namespace client
{
namespace ui
{

UI ui;

UI::UI() :
  fpsLabel( null ), root( null ), loadingScreen( null ), questFrame( null ), galileoFrame( null ),
  musicPlayer( null ), buildMenu( null ), debugFrame( null )
{}

void UI::showLoadingScreen( bool doShow )
{
  loadingScreen->raise();
  loadingScreen->show( doShow );
}

void UI::prepare()
{
  isFreelook = !mouse.doShow;

  foreach( area, root->children.iter() ) {
    if( !( area->flags & Area::PINNED_BIT ) ) {
      area->show( mouse.doShow );
    }
  }
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
  glViewport( 0, 0, camera.width, camera.height );

  tf.ortho( camera.width, camera.height );
  tf.camera = Mat44::ID;
  shader.colourTransform = Mat44::ID;

  // set shaders
  shader.setAmbientLight( Vec4( 0.55f, 0.55f, 0.55f, 1.00f ) );
  shader.setCaelumLight( ~Vec3( 0.6f, -0.3f, -0.8f ), Vec4( 0.45f, 0.45f, 0.45f, 1.00f ) );

  for( int i = 0; i < library.shaders.length(); ++i ) {
    shader.program( i );

    tf.applyCamera();
    shader.updateLights();

    glUniform1f( param.oz_Fog_dist, 1.0e6f );
  }

  glClear( GL_DEPTH_BUFFER_BIT );

  glEnable( GL_BLEND );

  shader.program( shader.plain );

  root->drawChildren();
  mouse.draw();

  if( showFPS ) {
    if( timer.frameTicks != 0 ) {
      fps = Math::mix( fps, 1.0f / timer.frameTime, 0.04f );
    }

    fpsLabel->setText( "%.1f", fps );
    fpsLabel->draw( root, true );
  }

  glDisable( GL_BLEND );

  OZ_GL_CHECK_ERROR();
}

void UI::load()
{
  isFreelook = false;

  questFrame   = new QuestFrame();
  galileoFrame = new GalileoFrame( questFrame );
  musicPlayer  = new MusicPlayer();
  buildMenu    = showBuild ? new BuildMenu() : null;
  debugFrame   = showDebug ? new DebugFrame() : null;

  root->add( galileoFrame );
  root->add( musicPlayer );
  root->add( questFrame );

  if( showBuild ) {
    root->add( buildMenu );
  }
  if( showDebug ) {
    root->add( debugFrame );
  }

  loadingScreen->raise();
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
  if( musicPlayer != null ) {
    root->remove( musicPlayer );
    musicPlayer = null;
  }
  if( galileoFrame != null ) {
    root->remove( galileoFrame );
    galileoFrame = null;
  }
  if( questFrame != null ) {
    root->remove( questFrame );
    questFrame = null;
  }
}

void UI::init()
{
  isFreelook = false;

  showFPS   = config.getSet( "ui.showFPS",   false );
  showBuild = config.getSet( "ui.showBuild", false );
  showDebug = config.getSet( "ui.showDebug", false );

  fps = 1.0f / Timer::TICK_TIME;

  font.init();
  mouse.load();

  root = new Area( camera.width, camera.height );
  loadingScreen = new LoadingArea();

  if( showFPS ) {
    fpsLabel = new Label( -4, -4, Area::ALIGN_RIGHT | Area::ALIGN_TOP, Font::MONO, " " );
  }

  root->add( loadingScreen );
}

void UI::free()
{
  delete fpsLabel;
  delete root;

  root     = null;
  fpsLabel = null;

  Area::updateAreas.clear();
  Area::updateAreas.dealloc();

  mouse.unload();
  font.free();
}

}
}
}
