/*
 *  QuestModule.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/modules/QuestModule.hpp"

#include "client/Lua.hpp"

#include "client/ui/UI.hpp"

#include "luamacros.hpp"

namespace oz
{
namespace client
{

QuestModule questModule;

Quest::Quest()
{}

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

void QuestModule::write( OutputStream* ostream ) const
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

  OZ_LUA_CONST( "OZ_QUEST_SUCCESSFUL", Quest::SUCCESSFUL );
  OZ_LUA_CONST( "OZ_QUEST_FAILED", Quest::FAILED );
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
