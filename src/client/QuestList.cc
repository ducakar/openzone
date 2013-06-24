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
 * @file client/QuestList.cc
 */

#include <client/QuestList.hh>

#include <client/ui/UI.hh>

namespace oz
{
namespace client
{

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

void QuestList::read( InputStream* istream )
{
  int nQuests = istream->readInt();

  for( int i = 0; i < nQuests; ++i ) {
    quests.add();
    Quest& quest = quests.last();

    quest.title       = istream->readString();
    quest.description = istream->readString();
    quest.place       = istream->readPoint();
    quest.state       = Quest::State( istream->readInt() );
  }

  activeQuest = istream->readInt();
}

void QuestList::write( OutputStream* ostream ) const
{
  ostream->writeInt( quests.length() );

  foreach( quest, quests.citer() ) {
    ostream->writeString( quest->title );
    ostream->writeString( quest->description );
    ostream->writePoint( quest->place );
    ostream->writeInt( quest->state );
  }

  ostream->writeInt( activeQuest );
}

void QuestList::load()
{
  activeQuest = -1;
}

void QuestList::unload()
{
  quests.clear();
  quests.deallocate();
}

QuestList questList;

}
}
