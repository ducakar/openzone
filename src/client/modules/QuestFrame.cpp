/*
 *  QuestFrame.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/modules/QuestFrame.hpp"

#include "client/Camera.hpp"

#include "client/ui/Button.hpp"

#include "client/modules/QuestModule.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  void QuestFrame::open( Button* sender )
  {
    QuestFrame* questFrame = static_cast<QuestFrame*>( sender->parent );

    if( questFrame->isOpened ) {
      questFrame->y += 300;
      questFrame->height -= 300;
      questFrame->isOpened = false;
    }
    else {
      questFrame->y -= 300;
      questFrame->height += 300;
      questFrame->isOpened = true;
    }
  }

  void QuestFrame::onDraw()
  {
    Frame::onDraw();
  }

  QuestFrame::QuestFrame() :
      Frame( 0, -8, 500, 0, gettext( "Quests" ) )
  {
    x = ( camera.width - width ) / 2;

    add( new Button( " + ", open, 16, 14 ), -20, -18 );

    if( !ui::mouse.doShow ) {
      show( false );
    }
  }

}
}
}
