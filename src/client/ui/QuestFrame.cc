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
 * @file client/ui/QuestFrame.cc
 */

#include "stable.hh"

#include "client/ui/QuestFrame.hh"

#include "client/Camera.hh"
#include "client/QuestList.hh"

namespace oz
{
namespace client
{
namespace ui
{

void QuestFrame::updateTask()
{
  const Quest& quest = questList.quests[currentQuest];

  String stateText;
  if( quest.state == Quest::PENDING ) {
    stateText = OZ_GETTEXT( "in progress" );
  }
  else if( quest.state == Quest::SUCCESSFUL ) {
    stateText = OZ_GETTEXT( "successful" );
  }
  else {
    stateText = OZ_GETTEXT( "failed" );
  }

  title.set( "%s  [%s]", quest.title.cstr(), stateText.cstr() );
  description.set( "%s", quest.description.cstr() );

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

  int nQuests = questList.quests.length();

  if( nQuests == 0 ) {
    questFrame->currentQuest = -1;
    questFrame->description.set( " " );
  }
  else {
    questFrame->currentQuest = ( questFrame->currentQuest + 1 + nQuests ) % nQuests;
    questFrame->updateTask();
  }
}

void QuestFrame::prev( Button* sender )
{
  QuestFrame* questFrame = static_cast<QuestFrame*>( sender->parent );

  int nQuests = questList.quests.length();

  if( nQuests == 0 ) {
    questFrame->currentQuest = -1;
    questFrame->description.set( " " );
  }
  else {
    questFrame->currentQuest = ( questFrame->currentQuest - 1 + nQuests + nQuests ) % nQuests;
    questFrame->updateTask();
  }
}

void QuestFrame::onDraw()
{
  if( currentQuest < 0 ) {
    if( !questList.quests.isEmpty() ) {
      currentQuest = 0;
      updateTask();
    }
  }
  else {
    const Quest& quest = questList.quests[currentQuest];

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
  Frame( 500, 0, OZ_GETTEXT( "Quests" ) ),
  description( 6, 4, 488, 10, Font::SANS ),
  lastState( Quest::PENDING ),
  isOpened( false ),
  currentQuest( -1 )
{
  contentHeight = 8 + 10 * font.INFOS[Font::SANS].height;

  x = ( camera.width - width ) / 2;

  y -= contentHeight;
  height += contentHeight;

  title.set( 16, -font.INFOS[Font::LARGE].height - 6, ALIGN_NONE, Font::LARGE, "%s",
             OZ_GETTEXT( "No quest" ) );

  add( new Button( " + ", open, 16, 14 ), -4, -4 );
  add( new Button( " > ", next, 16, 14 ), -28, -4 );
  add( new Button( " < ", prev, 16, 14 ), -48, -4 );

  y += contentHeight;
  height -= contentHeight;
}

void QuestFrame::clear()
{
  currentQuest = -1;
  description.clear();
}

}
}
}
