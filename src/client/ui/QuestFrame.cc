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
 * @file client/ui/QuestFrame.cc
 */

#include <stable.hh>
#include <client/ui/QuestFrame.hh>

#include <client/Camera.hh>
#include <client/ui/Style.hh>

namespace oz
{
namespace client
{
namespace ui
{

void QuestFrame::updateTask()
{
  if( questList.activeQuest < 0 ) {
    title.set( "%s", OZ_GETTEXT( "No quest" ) );
    description.set( " " );

    lastQuest = questList.activeQuest;
    lastState = Quest::NONE;
  }
  else {
    const Quest& quest = questList.quests[questList.activeQuest];

    if( quest.state == Quest::NONE ) {
      title.set( "%s", quest.title.cstr() );
    }
    else {
      const char* stateText;

      if( quest.state == Quest::PENDING ) {
        stateText = OZ_GETTEXT( "in progress" );
      }
      else if( quest.state == Quest::SUCCESSFUL ) {
        stateText = OZ_GETTEXT( "successful" );
      }
      else {
        stateText = OZ_GETTEXT( "failed" );
      }

      title.set( "%s  [%s]", quest.title.cstr(), stateText );
    }

    description.set( "%s", quest.description.cstr() );

    lastQuest = questList.activeQuest;
    lastState = quest.state;
  }
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

  if( nQuests > 1 ) {
    questList.activeQuest = ( questList.activeQuest + 1 ) % nQuests;
    questFrame->updateTask();
  }
}

void QuestFrame::prev( Button* sender )
{
  QuestFrame* questFrame = static_cast<QuestFrame*>( sender->parent );

  int nQuests = questList.quests.length();

  if( nQuests > 1 ) {
    questList.activeQuest = ( questList.activeQuest + nQuests - 1 ) % nQuests;
    questFrame->updateTask();
  }
}

void QuestFrame::onUpdate()
{
  if( lastQuest != questList.activeQuest ||
      ( lastQuest >= 0 && lastState != questList.quests[lastQuest].state ) )
  {
    updateTask();
  }
}

void QuestFrame::onDraw()
{
  Frame::onDraw();

  if( isOpened ) {
    description.draw( this, false );
  }
}

QuestFrame::QuestFrame() :
  Frame( 500, 2, OZ_GETTEXT( "No quest" ) ),
  description( 6, 4, 488, 16, Font::SANS, Area::ALIGN_NONE ),
  contentHeight( 8 + 16 * style.fonts[Font::SANS].height ),
  isOpened( false ), lastQuest( -1 ), lastState( Quest::NONE )
{
  flags |= UPDATE_BIT;

  height += contentHeight;

  add( new Button( " + ", open, 24, 16 ), -4, 4 );
  add( new Button( " > ", next, 24, 16 ), -36, 4 );
  add( new Button( " < ", prev, 24, 16 ), -64, 4 );

  height -= contentHeight;
}

void QuestFrame::clear()
{
  description.clear();
}

}
}
}
