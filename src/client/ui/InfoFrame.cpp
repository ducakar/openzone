/*
 *  InfoFrame.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ui/InfoFrame.hpp"

#include "nirvana/Memo.hpp"
#include "nirvana/Nirvana.hpp"

#include "client/Camera.hpp"

using namespace oz::nirvana;

namespace oz
{
namespace client
{
namespace ui
{

  InfoFrame::InfoFrame() :
      Frame( -8, -8, 308, 8 + 15 * ( Font::INFOS[Font::MONO].height + 2 ), "" ),
      text( 4, 4, 300, 15, Font::SANS )
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
        !nirvana::nirvana.devices.contains( camera.tagged ) )
    {
      return false;
    }

    return true;
  }

  void InfoFrame::onDraw()
  {
    if( camera.state != Camera::BOT || camera.botObj == null || camera.tagged == -1 ||
        !nirvana::nirvana.devices.contains( camera.tagged ) )
    {
      return;
    }

    if( lastId != camera.tagged ) {
      const Device* device = nirvana::nirvana.devices.get( camera.tagged );

      text.setText( "%s", device->getMemo() );

      lastId = camera.tagged;
    }

    Frame::onDraw();
    text.draw( this );
  }

}
}
}
