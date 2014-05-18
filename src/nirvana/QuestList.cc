/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file nirvana/QuestList.cc
 */

#include <nirvana/QuestList.hh>

namespace oz
{

QuestList::QuestList() :
  activeQuest( -1 )
{}

void QuestList::add( const char* title, const char* description, const Point& place,
                     Quest::State state )
{
  if( quests.isEmpty() ) {
    activeQuest = 0;
  }

  Quest quest = { title, description, place, state };
  quests.add( static_cast<Quest&&>( quest ) );
}

void QuestList::remove( int index )
{
  quests.erase( index );

  if( index < activeQuest ) {
    --activeQuest;

    if( quests.isEmpty() ) {
      activeQuest = -1;
    }
  }
}

void QuestList::read( InputStream* is )
{
  int nQuests = is->readInt();

  for( int i = 0; i < nQuests; ++i ) {
    quests.add( Quest() );
    Quest& quest = quests.last();

    quest.title       = is->readString();
    quest.description = is->readString();
    quest.place       = is->readPoint();
    quest.state       = Quest::State( is->readInt() );
  }

  activeQuest = is->readInt();
}

void QuestList::write( OutputStream* os ) const
{
  os->writeInt( quests.length() );

  for( const Quest& quest : quests ) {
    os->writeString( quest.title );
    os->writeString( quest.description );
    os->writePoint( quest.place );
    os->writeInt( quest.state );
  }

  os->writeInt( activeQuest );
}

void QuestList::load()
{}

void QuestList::unload()
{
  quests.clear();
  quests.trim();

  activeQuest = -1;
}

QuestList questList;

}
