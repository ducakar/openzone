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
#include "client/ui/StrategicArea.hh"
#include "client/ui/HudArea.hh"
#include "client/ui/QuestFrame.hh"
#include "client/ui/GalileoFrame.hh"
#include "client/ui/MusicPlayer.hh"
#include "client/ui/Inventory.hh"
#include "client/ui/InfoFrame.hh"
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
  fpsLabel( null ), root( null ), loadingScreen( null ), strategicArea( null ), hudArea( null ),
  questFrame( null ), galileoFrame( null ), musicPlayer( null ), inventory( null ),
  buildMenu( null ), debugFrame( null )
{}

void UI::showLoadingScreen( bool doShow )
{
  loadingScreen->raise();
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

  Area::update();

  if( !isFreelook ) {
    root->passMouseEvents();
  }
}

void UI::draw()
{
  glViewport( 0, 0, camera.width, camera.height );
  glClear( GL_DEPTH_BUFFER_BIT );

  shape.bind();

  tf.ortho( camera.width, camera.height );
  tf.camera = Mat44::ID;
  tf.colour = Mat44::ID;

  // set shaders
  shader.setAmbientLight( Vec4( 0.5f, 0.5f, 0.5f, 1.00f ) );
  shader.setCaelumLight( ~Vec3( 1.0f, -1.0f, -1.5f ), Vec4( 0.5f, 0.5f, 0.5f, 1.0f ) );

  for( int i = 0; i < library.shaders.length(); ++i ) {
    shader.program( i );

    tf.applyCamera();
    shader.updateLights();

    glUniform1f( param.oz_Fog_dist, 1.0e6f );
  }

  shader.program( shader.plain );

  root->drawChildren();
  mouse.draw();

  if( showFPS ) {
    if( timer.frameTicks != 0 ) {
      fps = Math::mix( fps, 1.0f / timer.frameTime, 0.04f );
    }

    fpsLabel->set( "%.1f", fps );
    fpsLabel->draw( root, true );
  }

  shape.unbind();

  OZ_GL_CHECK_ERROR();
}

void UI::load()
{
  isFreelook = false;

  strategicArea = new StrategicArea();
  hudArea       = new HudArea();
  questFrame    = new QuestFrame();
  galileoFrame  = new GalileoFrame();
  musicPlayer   = new MusicPlayer();
  inventory     = new Inventory();
  infoFrame     = new InfoFrame();
  buildMenu     = showBuild ? new BuildMenu() : null;
  debugFrame    = showDebug ? new DebugFrame() : null;

  root->add( strategicArea, 0, 0 );
  root->add( hudArea, 0, 0 );
  root->add( questFrame, Area::CENTRE, -8 );
  root->add( galileoFrame, 8, -8 );
  root->add( musicPlayer, 8, -16 - galileoFrame->height );
  root->add( inventory, Area::CENTRE, 8 );
  root->add( infoFrame, -8, -8 );

  if( showBuild ) {
    root->add( buildMenu, 8, -24 - galileoFrame->height - musicPlayer->height );
  }
  if( showDebug ) {
    root->add( debugFrame, Area::CENTRE, -16 - questFrame->height );
  }

  strategicArea->enable( false );
  hudArea->enable( false );
  inventory->enable( false );
  infoFrame->enable( false );

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
  if( infoFrame != null ) {
    root->remove( infoFrame );
    infoFrame = null;
  }
  if( inventory != null ) {
    root->remove( inventory );
    inventory = null;
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
  if( hudArea != null ) {
    root->remove( hudArea );
    hudArea = null;
  }
  if( strategicArea != null ) {
    root->remove( strategicArea );
    strategicArea = null;
  }
}

void UI::init()
{
  isFreelook = false;

  showFPS   = config.include( "ui.showFPS",   false ).asBool();
  showBuild = config.include( "ui.showBuild", false ).asBool();
  showDebug = config.include( "ui.showDebug", false ).asBool();

  fps = 1.0f / Timer::TICK_TIME;

  font.init();
  mouse.init();

  root = new Area( camera.width, camera.height );
  loadingScreen = new LoadingArea();

  if( showFPS ) {
    fpsLabel = new Label( -4, -4, Area::ALIGN_RIGHT | Area::ALIGN_TOP, Font::MONO, " " );
  }

  root->add( loadingScreen, 0, 0 );
}

void UI::free()
{
  delete fpsLabel;
  delete root;

  root     = null;
  fpsLabel = null;

  Area::updateAreas.clear();
  Area::updateAreas.dealloc();

  mouse.free();
  font.free();
}

}
}
}
