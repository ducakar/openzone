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

#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  String MainMenu::autosaveFile;
  String MainMenu::quicksaveFile;

  static void continueAutosaved( ui::Button* )
  {
    Stage::nextStage = &gameStage;
    gameStage.onCreate = "";
    gameStage.stateFile = GameStage::AUTOSAVE_FILE;
  }

  static void continueQuicksaved( ui::Button* )
  {
    Stage::nextStage = &gameStage;
    gameStage.onCreate = "";
    gameStage.stateFile = GameStage::QUICKSAVE_FILE;
  }

  static void loadTutorial( ui::Button* )
  {
    Stage::nextStage = &gameStage;
    gameStage.onCreate = "init_tutorial";
  }

  static void loadSample( ui::Button* )
  {
    Stage::nextStage = &gameStage;
    gameStage.onCreate = "init_sample";
  }

  static void loadTest( ui::Button* )
  {
    Stage::nextStage = &gameStage;
    gameStage.onCreate = "init_test";
  }

  static void exit( ui::Button* )
  {
    menuStage.doExit = true;
  }

  MainMenu::MainMenu() : Frame( 0, 0, 400, 450, gettext( "Main Menu" ) ),
      copyright( 10, 10, 380, 10, Font::SANS )
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
    add( new Button( gettext( "Sample Mission" ), loadSample, 300, 20 ), 50, -180 );
    add( new Button( gettext( "Test World" ), loadTest, 300, 20 ), 50, -210 );
    add( new Button( gettext( "Exit" ), exit, 300, 20 ), 50, -270 );
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
