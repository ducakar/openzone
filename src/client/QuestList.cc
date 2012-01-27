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
 * @file client/QuestList.cc
 */

#include "stable.hh"

#include "client/QuestList.hh"

#include "client/ui/UI.hh"

namespace oz
{
namespace client
{

QuestList questList;

Quest::Quest( const char* title_, const char* description_, const Point3& place_, int state_ ) :
  title( title_ ), description( description_ ), place( place_ ), state( state_ )
{}

void QuestList::read( InputStream* istream )
{
  int nQuests = istream->readInt();
  for( int i = 0; i < nQuests; ++i ) {
    String title       = istream->readString();
    String description = istream->readString();
    Point3 place       = istream->readPoint3();
    int    state       = istream->readInt();

    quests.add( Quest( title, description, place, state ) );
  }
}

void QuestList::write( BufferStream* ostream ) const
{
  ostream->writeInt( quests.length() );
  foreach( quest, quests.citer() ) {
    ostream->writeString( quest->title );
    ostream->writeString( quest->description );
    ostream->writePoint3( quest->place );
    ostream->writeInt( quest->state );
  }
}

void QuestList::load()
{}

void QuestList::unload()
{
  quests.clear();
  quests.dealloc();
}

}
}
