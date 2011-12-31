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
 * @file client/modules/QuestModule.cc
 */

#include "stable.hh"

#include "client/modules/QuestModule.hh"

#include "client/Lua.hh"

#include "client/ui/UI.hh"

#include "luamacros.hh"

namespace oz
{
namespace client
{

QuestModule questModule;

Quest::Quest( const char* title_, const char* description_, const Point3& place_, int state_ ) :
    title( title_ ), description( description_ ), place( place_ ), state( state_ )
{}

void QuestModule::read( InputStream* istream )
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

void QuestModule::write( BufferStream* ostream ) const
{
  ostream->writeInt( quests.length() );
  foreach( quest, quests.citer() ) {
    ostream->writeString( quest->title );
    ostream->writeString( quest->description );
    ostream->writePoint3( quest->place );
    ostream->writeInt( quest->state );
  }
}

void QuestModule::load()
{
  questFrame = new ui::QuestFrame();
  ui::ui.root->add( questFrame );
  ui::ui.root->focus( ui::ui.loadingScreen );
}

void QuestModule::unload()
{
  quests.clear();
  quests.dealloc();

  if( questFrame != null ) {
    ui::ui.root->remove( questFrame );
    questFrame = null;
  }
}

void QuestModule::registerLua() const
{
  OZ_LUA_FUNC( ozQuestAdd );
  OZ_LUA_FUNC( ozQuestEnd );
}

void QuestModule::init()
{
  questFrame = null;
}

int QuestModule::ozQuestAdd( lua_State* l )
{
  ARG( 5 );

  questModule.quests.add( Quest( tostring( 1 ),
                                 tostring( 2 ),
                                 Point3( tofloat( 3 ), tofloat( 4 ), tofloat( 5 ) ),
                                 Quest::PENDING ) );

  pushint( questModule.quests.length() - 1 );
  return 1;
}

int QuestModule::ozQuestEnd( lua_State* l )
{
  ARG( 2 );

  int id = toint( 1 );
  if( uint( id ) >= uint( questModule.quests.length() ) ) {
    ERROR( "invalid quest id" );
  }

  questModule.quests[id].state = tobool( 2 ) ? Quest::SUCCESSFUL : Quest::FAILED;
  return 0;
}

}
}
