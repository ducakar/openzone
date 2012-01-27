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
 * @file client/ui/InfoFrame.cc
 */

#include "stable.hh"

#include "client/ui/InfoFrame.hh"

#include "nirvana/Memo.hh"
#include "nirvana/Nirvana.hh"

#include "client/Camera.hh"

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

bool InfoFrame::onMouseEvent()
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
    lastId = -1;
    return;
  }

  const Device* const* device = nirvana.devices.find( camera.tagged );
  if( device == null ) {
    lastId = -1;
    return;
  }

  if( lastId != camera.tagged ) {
    lastId = camera.tagged;
    lastTicks = timer.ticks;

    const Bot* tagged = static_cast<const Bot*>( camera.taggedObj );

    text.setText( "%s", ( *device )->getMemo() );

    if( tagged->flags & Object::BOT_BIT ) {
      title.setText( "%s (%s)", tagged->name.cstr(), tagged->clazz->title.cstr() );
    }
    else {
      title.setText( "%s", tagged->clazz->title.cstr() );
    }
  }

  Frame::onDraw();
  text.draw( this );
}

}
}
}
