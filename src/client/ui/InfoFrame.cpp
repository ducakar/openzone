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
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file client/ui/InfoFrame.cpp
 */

#include "stable.hpp"

#include "client/ui/InfoFrame.hpp"

#include "nirvana/Memo.hpp"
#include "nirvana/Nirvana.hpp"

#include "client/Camera.hpp"

namespace oz
{
namespace client
{
namespace ui
{

using namespace oz::nirvana;
using oz::nirvana::nirvana;

InfoFrame::InfoFrame() :
    Frame( -8, -8, 360, 12 + 24 * Font::INFOS[Font::SANS].height, "" ),
    text( 6, 4, 348, 24, Font::SANS ), lastId( -1 )
{
  flags = PINNED_BIT;
}

void InfoFrame::onVisibilityChange()
{
  lastId = -1;
}

bool InfoFrame::onMouseOver()
{
  if( camera.state != Camera::BOT || camera.botObj == null || camera.tagged == -1 ||
      !nirvana.devices.contains( camera.tagged ) )
  {
    return false;
  }

  return true;
}

void InfoFrame::onDraw()
{
  if( camera.state != Camera::BOT || camera.botObj == null || camera.tagged == -1 ) {
    return;
  }

  const Device* const* device = nirvana.devices.find( camera.tagged );
  if( device == null ) {
    return;
  }

  if( lastId != camera.tagged || uint( timer.ticks - lastTicks ) >= uint( REFRESH_INTERVAL ) ) {
    const Bot* tagged = static_cast<const Bot*>( camera.taggedObj );

    if( tagged->flags & Object::BOT_BIT ) {
      title.setText( "%s (%s)", tagged->name.cstr(), tagged->clazz->title.cstr() );
    }
    else {
      title.setText( "%s", tagged->clazz->title.cstr() );
    }
    text.setText( "%s", ( *device )->getMemo() );

    lastId = camera.tagged;
    lastTicks = timer.ticks;
  }

  Frame::onDraw();
  text.draw( this );
}

}
}
}
