/*
 *  GalileoFrame.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ui/GalileoFrame.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  GalileoFrame::GalileoFrame() :
      Frame( 8, -8, 400, 400, "" )
  {
    flags = PINNED_BIT;

    show( false );
  }

  void GalileoFrame::onDraw()
  {
    Frame::onDraw();
  }

}
}
}
