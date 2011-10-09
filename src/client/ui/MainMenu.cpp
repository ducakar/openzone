/*
 *  MainMenu.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ui/MainMenu.hpp"

#include "client/Camera.hpp"
#include "client/Shader.hpp"
#include "client/Shape.hpp"
#include "client/GameStage.hpp"
#include "client/MenuStage.hpp"

#include "client/ui/SettingsFrame.hpp"
#include "client/ui/UI.hpp"

#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  String MainMenu::autosaveFile;
  String MainMenu::quicksaveFile;

  static void continueAutosaved( Button* sender )
  {
    ui.root->remove( sender->parent );

    Stage::nextStage = &gameStage;
    gameStage.stateFile = GameStage::AUTOSAVE_FILE;
  }

  static void continueQuicksaved( Button* sender )
  {
    ui.root->remove( sender->parent );

    Stage::nextStage = &gameStage;
    gameStage.stateFile = GameStage::QUICKSAVE_FILE;
  }

  static void loadTutorial( Button* sender )
  {
    ui.root->remove( sender->parent );

    Stage::nextStage = &gameStage;
    gameStage.stateFile = "";
    gameStage.missionFile = "tutorial";
  }

  static void loadTest( Button* sender )
  {
    ui.root->remove( sender->parent );

    Stage::nextStage = &gameStage;
    gameStage.stateFile = "";
    gameStage.missionFile = "test";
  }

  static void loadCvicek( Button* sender )
  {
    ui.root->remove( sender->parent );

    Stage::nextStage = &gameStage;
    gameStage.stateFile = "";
    gameStage.missionFile = "cvicek";
  }

  static void settings( Button* sender )
  {
    ui.root->remove( sender->parent );
    ui.root->add( new SettingsFrame() );
  }

  static void exit( Button* sender )
  {
    ui.root->remove( sender->parent );
    menuStage.doExit = true;
  }

  MainMenu::MainMenu() : Frame( 0, 0, 400, 450, OZ_APPLICATION_TITLE " v" OZ_APPLICATION_VERSION ),
      copyright( 10, 10, 380, 9, Font::SANS )
  {
    x = ( camera.width - width ) / 2;
    y = ( camera.height - height ) / 2;

    copyright.setText(
        String( "OpenZone  Copyright © 2002-2011  Davorin Učakar" ) +
        gettext( "\n\nThis program comes with ABSOLUTELY NO WARRANTY. "
                 "This is free software, and you are welcome to redistribute it "
                 "under certain conditions; See COPYING file for details.\n\n"
                 "Data files come form different sources. See respective README and COPYING "
                 "files for details about copyrights and licences." ) );

    File autosaveFile( config.get( "dir.rc", "" ) + String( "/autosave.ozState" ) );
    File quicksaveFile( config.get( "dir.rc", "" ) + String( "/quicksave.ozState" ) );

    if( autosaveFile.getType() == File::REGULAR ) {
      MainMenu::autosaveFile = autosaveFile.path();
      add( new Button( gettext( "Continue" ), continueAutosaved, 300, 20 ), 50, -60 );
    }
    if( quicksaveFile.getType() == File::REGULAR ) {
      MainMenu::quicksaveFile = quicksaveFile.path();
      add( new Button( gettext( "Load Quicksave" ), continueQuicksaved, 300, 20 ), 50, -90 );
    }

    add( new Button( gettext( "Tutorial" ), loadTutorial, 300, 20 ), 50, -150 );
    add( new Button( gettext( "Test World" ), loadTest, 300, 20 ), 50, -180 );
    add( new Button( gettext( "Mission 1: Cvicek" ), loadCvicek, 300, 20 ), 50, -210 );

    add( new Button( gettext( "Settings" ), settings, 300, 20 ), 50, -270 );
    add( new Button( gettext( "Exit" ), exit, 300, 20 ), 50, -300 );
  }

  MainMenu::~MainMenu()
  {
    autosaveFile.clear();;
    quicksaveFile.clear();
  }

  void MainMenu::onDraw()
  {
    glUniform4f( param.oz_Colour, 0.1f, 0.1f, 0.1f, 1.0f );
    shape.fill( 0, 0, camera.width, camera.height );

    Frame::onDraw();

    copyright.draw( this );
  }

}
}
}
