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
 * @file client/ui/SettingsFrame.cpp
 */

#include "stable.hpp"

#include "client/ui/SettingsFrame.hpp"

#include "client/Shader.hpp"
#include "client/Shape.hpp"
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
  shape.fill( 0, 0, Area::uiWidth, Area::uiHeight );

  Frame::onDraw();

  message.draw( this );
}

SettingsFrame::SettingsFrame() :
    Frame( 0, 0, 400, 28 + 8 * font.INFOS[Font::SANS].height, gettext( "Settings" ) ),
    message( 4, 24, 392, 8, Font::SANS )
{
  x = ( Area::uiWidth  - width ) / 2;
  y = ( Area::uiHeight - height ) / 2;

  message.setText( gettext( "NOT IMPLEMENTED YET\n\n"
      "You can change your settings by manually editing '%s' file." ),
      ( config.get( "dir.rc", "" ) + String( "/client.rc" ) ).cstr() );

  add( new Button( gettext( "Close" ), closeFrame, 40, 16 ), -44, 4 );
}

}
}
}
