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
 * @file client/modules/QuestFrame.cpp
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

void QuestFrame::updateTask()
{
  const Quest& quest = questModule.quests[currentQuest];

  String stateText;
  if( quest.state == Quest::PENDING ) {
    stateText = gettext( "in progress" );
  }
  else if( quest.state == Quest::SUCCESSFUL ) {
    stateText = gettext( "successful" );
  }
  else {
    stateText = gettext( "failed" );
  }

  title.setText( "%s  [%s]", quest.title.cstr(), stateText.cstr() );
  description.setText( "%s", quest.description.cstr() );

  lastState = quest.state;
}

void QuestFrame::open( Button* sender )
{
  QuestFrame* questFrame = static_cast<QuestFrame*>( sender->parent );

  if( questFrame->isOpened ) {
    questFrame->y += questFrame->contentHeight;
    questFrame->height -= questFrame->contentHeight;
    questFrame->isOpened = false;
  }
  else {
    questFrame->y -= questFrame->contentHeight;
    questFrame->height += questFrame->contentHeight;
    questFrame->isOpened = true;
  }
}

void QuestFrame::next( Button* sender )
{
  QuestFrame* questFrame = static_cast<QuestFrame*>( sender->parent );

  int nQuests = questModule.quests.length();

  if( nQuests == 0 ) {
    questFrame->currentQuest = -1;
    questFrame->description.setText( " " );
  }
  else {
    questFrame->currentQuest = ( questFrame->currentQuest + 1 + nQuests ) % nQuests;
    questFrame->updateTask();
  }
}

void QuestFrame::prev( Button* sender )
{
  QuestFrame* questFrame = static_cast<QuestFrame*>( sender->parent );

  int nQuests = questModule.quests.length();

  if( nQuests == 0 ) {
    questFrame->currentQuest = -1;
    questFrame->description.setText( " " );
  }
  else {
    questFrame->currentQuest = ( questFrame->currentQuest - 1 + nQuests + nQuests ) % nQuests;
    questFrame->updateTask();
  }
}

void QuestFrame::onDraw()
{
  if( currentQuest == -1 ) {
    if( !questModule.quests.isEmpty() ) {
      currentQuest = 0;
      updateTask();
    }
  }
  else {
    const Quest& quest = questModule.quests[currentQuest];

    if( quest.state != lastState ) {
      updateTask();
    }
  }

  Frame::onDraw();

  if( isOpened ) {
    description.draw( this );
  }
}

QuestFrame::QuestFrame() :
    Frame( 0, -8, 500, 0, gettext( "Quests" ) ),
    description( 6, 4, 488, 10, Font::SANS ),
    lastState( Quest::PENDING ),
    currentQuest( -1 ),
    isOpened( false )
{
  if( !mouse.doShow ) {
    flags = HIDDEN_BIT | IGNORE_BIT;
  }

  contentHeight = 8 + 10 * font.INFOS[Font::SANS].height;

  x = ( Area::uiWidth - width ) / 2;

  y -= contentHeight;
  height += contentHeight;

  title.set( 16, -font.INFOS[Font::LARGE].height - 6, ALIGN_NONE, Font::LARGE, "%s",
             gettext( "No Quest" ) );

  add( new Button( " + ", open, 16, 14 ), -20, -18 );
  add( new Button( " > ", next, 16, 14 ), -44, -18 );
  add( new Button( " < ", prev, 16, 14 ), -64, -18 );

  y += contentHeight;
  height -= contentHeight;
}

}
}
}
