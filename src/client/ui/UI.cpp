/*
 *  UI.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ui/UI.hpp"

#include "client/Camera.hpp"
#include "client/Colours.hpp"
#include "client/Context.hpp"
#include "client/Shape.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  UI ui;

#ifndef NDEBUG
  UI::UI() : root( null ), loadingScreen( null ), hudArea( null ), strategicArea( null ),
      inventoryMenu( null ), musicPlayer( null ), buildMenu( null ), debugFrame( null )
  {}
#else
  UI::UI() : root( null ), loadingScreen( null ), hudArea( null ), strategicArea( null ),
      inventoryMenu( null ), musicPlayer( null ), buildMenu( null )
  {}
#endif

  void UI::showLoadingScreen( bool doShow )
  {
    root->focus( loadingScreen );
    loadingScreen->show( doShow );
  }

  void UI::update()
  {
    if( keyboard.keys[SDLK_TAB] & ~keyboard.oldKeys[SDLK_TAB] ) {
      mouse.doShow = !mouse.doShow;
    }
    if( mouse.doShow != isFreelook ) {
      isFreelook = mouse.doShow;

      foreach( area, root->children.iter() ) {
        if( !( area->flags & Area::PINNED_BIT ) ) {
          area->show( isFreelook );
        }
      }
    }
    if( isFreelook ) {
      root->passMouseEvents();
    }
    Area::update();
  }

  void UI::draw()
  {
    hard_assert( glGetError() == GL_NO_ERROR );

    tf.ortho();
    tf.camera = Mat44::ID;

    // set shaders
    for( int i = 0; i < translator.shaders.length(); ++i ) {
      if( shader.isLoaded || i == shader.ui ) {
        shader.use( i );

        tf.applyCamera();

        shader.setAmbientLight( Vec4( 0.6f, 0.5f, 0.6f, 1.0f ) );
        shader.setCaelumLight( Vec3( 0.67f, -0.67f, -0.33f ), Vec4( 0.6f, 0.6f, 0.6f, 1.0f ) );
        shader.updateLights();

        glUniform1f( param.oz_Fog_start, 1000000.0f );
        glUniform1f( param.oz_Fog_end, 2000000.0f );
      }
    }

    shader.use( shader.ui );

    shape.bindVertexArray();

    glClear( GL_DEPTH_BUFFER_BIT );
    glEnable( GL_BLEND );

    root->drawChildren();
    mouse.draw();

    glDisable( GL_BLEND );

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void UI::load()
  {
    isFreelook = false;

    mouse.load();

    try {
      strategicArea = new StrategicArea();
      hudArea       = new HudArea();
      inventoryMenu = new InventoryMenu();
      musicPlayer   = new MusicPlayer();
      buildMenu     = new BuildMenu();
#ifndef NDEBUG
      debugFrame    = new DebugFrame();
#endif
    }
    catch( ... ) {
      delete strategicArea;
      delete hudArea;
      delete inventoryMenu;
      delete musicPlayer;
      delete buildMenu;
#ifndef NDEBUG
      delete debugFrame;
#endif

      strategicArea = null;
      hudArea       = null;
      inventoryMenu = null;
      musicPlayer   = null;
      buildMenu     = null;
#ifndef NDEBUG
      debugFrame    = null;
#endif

      throw;
    }

    root->add( strategicArea );
    root->add( hudArea );
    root->add( inventoryMenu );
    root->add( musicPlayer );
    root->add( buildMenu );
#ifndef NDEBUG
    root->add( debugFrame );
#endif

    root->focus( loadingScreen );
  }

  void UI::unload()
  {
#ifndef NDEBUG
    if( debugFrame != null ) {
      root->remove( debugFrame );
      debugFrame = null;
    }
#endif
    if( buildMenu != null ) {
      root->remove( buildMenu );
      buildMenu = null;
    }
    if( musicPlayer != null ) {
      root->remove( musicPlayer );
      musicPlayer = null;
    }
    if( inventoryMenu != null ) {
      root->remove( inventoryMenu );
      inventoryMenu = null;
    }
    if( hudArea != null ) {
      root->remove( hudArea  );
      hudArea = null;
    }
    if( strategicArea != null ) {
      root->remove( strategicArea );
      strategicArea = null;
    }

    mouse.unload();
  }

  void UI::init()
  {
    mouse.init();

    if( !font.init() ) {
      throw Exception( "Failed to load font" );
    }

    isFreelook = false;

    root = new Area( camera.width, camera.height );
    loadingScreen = new LoadingArea();

    root->add( loadingScreen );

    hard_assert( ui::ui.strategicArea == null );
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
