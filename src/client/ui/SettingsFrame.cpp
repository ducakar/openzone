/*
 *  SettingsFrame.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ui/SettingsFrame.hpp"

#include "client/Shader.hpp"
#include "client/Shape.hpp"
#include "client/Camera.hpp"
#include "client/OpenGL.hpp"

#include "client/ui/MainMenu.hpp"
#include "client/ui/UI.hpp"

namespace oz
{
namespace client
{
namespace ui
{

static void closeFrame( Button* sender )
{
  ui.root->remove( sender->parent );
  ui.root->add( new MainMenu() );
}

void SettingsFrame::onDraw()
{
  glUniform4f( param.oz_Colour, 0.1f, 0.1f, 0.1f, 1.0f );
  shape.fill( 0, 0, camera.width, camera.height );

  Frame::onDraw();

  message.draw( this );
}

SettingsFrame::SettingsFrame() :
    Frame( 0, 0, 400, 28 + 8 * font.INFOS[Font::SANS].height, gettext( "Settings" ) ),
    message( 4, 24, 392, 8, Font::SANS )
{
  x = ( camera.width - width ) / 2;
  y = ( camera.height - height ) / 2;

  message.setText( gettext( "NOT IMPLEMENTED YET\n\n"
      "You can change your settings by manually editing '%s' file." ),
      ( config.get( "dir.rc", "" ) + String( "/client.rc" ) ).cstr() );

  add( new Button( gettext( "Close" ), closeFrame, 40, 16 ), -4, 4 );
}

}
}
}
