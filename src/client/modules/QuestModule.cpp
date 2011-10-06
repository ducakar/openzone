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

#include "luamacros.hpp"

namespace oz
{
namespace client
{

  QuestModule questModule;

  Quest::Quest( const char* title_, const char* description_, int state_ ) :
      title( title_ ), description( description_ ), state( state_ )
  {}

  void QuestModule::read( InputStream* istream )
  {
    int nQuests = istream->readInt();
    for( int i = 0; i < nQuests; ++i ) {
      String id          = istream->readString();
      String title       = istream->readString();
      String description = istream->readString();
      int    state       = istream->readInt();

      quests.add( id, Quest( title, description, state ) );
    }
  }

  void QuestModule::write( OutputStream* ostream ) const
  {
    ostream->writeInt( quests.length() );
    foreach( quest, quests.citer() ) {
      ostream->writeString( quest.key() );
      ostream->writeString( quest.value().title );
      ostream->writeString( quest.value().description );
      ostream->writeInt( quest.value().state );
    }
  }

  void QuestModule::unload()
  {
    quests.clear();
    quests.dealloc();
  }

  void QuestModule::init()
  {
    OZ_LUA_FUNC( ozQuestAdd );
    OZ_LUA_FUNC( ozQuestEnd );

    OZ_LUA_CONST( "OZ_QUEST_SUCCESSFUL", Quest::SUCCESSFUL );
    OZ_LUA_CONST( "OZ_QUEST_FAILED", Quest::FAILED );
  }

  int QuestModule::ozQuestAdd( lua_State* l )
  {
    ARG( 3 );

    const char* id = tostring( 1 );
    if( questModule.quests.contains( id ) ) {
      ERROR( "quest id already exists" );
    }

    questModule.quests.add( tostring( 1 ), Quest( tostring( 2 ), tostring( 3 ), Quest::PENDING ) );
    return 0;
  }

  int QuestModule::ozQuestEnd( lua_State* l )
  {
    ARG( 2 );

    Quest* quest = questModule.quests.find( tostring( 1 ) );
    if( quest == null ) {
      ERROR( "invalid quest id" );
    }

    quest->state = toint( 2 );
    return 0;
  }

}
}
