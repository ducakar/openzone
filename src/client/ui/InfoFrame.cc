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

#include <stable.hh>
#include <client/ui/InfoFrame.hh>

#include <common/Timer.hh>
#include <nirvana/Memo.hh>
#include <nirvana/Nirvana.hh>
#include <client/Camera.hh>
#include <client/ui/Style.hh>

namespace oz
{
namespace client
{
namespace ui
{

using nirvana::nirvana;

void InfoFrame::onVisibilityChange( bool )
{
  lastId = -1;
}

bool InfoFrame::onMouseEvent()
{
  if( camera.state != Camera::UNIT || camera.botObj == nullptr || camera.object < 0 ||
      !nirvana.devices.contains( camera.object ) )
  {
    return false;
  }
  return true;
}

void InfoFrame::onDraw()
{
  if( camera.state != Camera::UNIT || camera.botObj == nullptr || camera.object < 0 ) {
    lastId = -1;
    return;
  }

  const Device* const* device = nirvana.devices.find( camera.object );
  if( device == nullptr ) {
    lastId = -1;
    return;
  }

  if( lastId != camera.object ) {
    lastId = camera.object;
    lastTicks = timer.ticks;

    const Bot* tagged = static_cast<const Bot*>( camera.objectObj );

    text.set( "%s", ( *device )->getMemo() );

    if( tagged->flags & Object::BOT_BIT ) {
      title.set( "%s (%s)", tagged->name.cstr(), tagged->clazz->title.cstr() );
    }
    else {
      title.set( "%s", tagged->clazz->title.cstr() );
    }
  }

  Frame::onDraw();
  text.draw( this, false );
}

InfoFrame::InfoFrame() :
  Frame( 360, 12 + 24 * style.fonts[Font::SANS].height, "" ),
  text( 6, 4, 348, 24, Font::SANS, Area::ALIGN_NONE ), lastId( -1 )
{
  flags = PINNED_BIT;
}

}
}
}
