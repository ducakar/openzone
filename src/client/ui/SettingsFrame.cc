/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file client/ui/SettingsFrame.cc
 */

#include <client/ui/SettingsFrame.hh>

#include <common/Lingua.hh>
#include <client/ui/Style.hh>
#include <client/ui/Button.hh>

namespace oz
{
namespace client
{
namespace ui
{

static void closeFrame( Button* sender )
{
  SettingsFrame* settings = static_cast<SettingsFrame*>( sender->parent );
  settings->parent->remove( settings );
}

void SettingsFrame::onDraw()
{
  Frame::onDraw();

  message.draw( this );
}

SettingsFrame::SettingsFrame() :
  Frame( 400, 40 + 8 * style.fonts[Font::SANS].height, OZ_GETTEXT( "Settings" ) ),
  message( 4, 24, -HEADER_SIZE - 4, Area::ALIGN_NONE, Font::SANS, "" )
{
  x = ( parent->width  - width ) / 2;
  y = ( parent->height - height ) / 2;

  const String& configDirPath = config["dir.config"].asString();

  message.setText( "%s\n\n  %s\n  %s\n  %s",
                   OZ_GETTEXT( "NOT IMPLEMENTED.\nYou can change your settings by manually editing"
                               " the following files:" ),
                   ( configDirPath + "/config.json" ).cstr(),
                   ( configDirPath + "/input.json" ).cstr(),
                   ( configDirPath + "/profile.json" ).cstr() );

  add( new Button( OZ_GETTEXT( "Close" ), closeFrame, 80, 25 ), -4, 4 );
}

}
}
}
