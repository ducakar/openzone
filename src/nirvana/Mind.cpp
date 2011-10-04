/*
 *  Mind.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "nirvana/Mind.hpp"

#include "nirvana/Lua.hpp"
#include "matrix/Bot.hpp"
#include "matrix/BotClass.hpp"

namespace oz
{
namespace nirvana
{

  Pool<Mind, 1024> Mind::pool;

  Mind::Mind( int bot_ ) : flags( 0 ), bot( bot_ )
  {
    lua.registerMind( bot );
  }

  Mind::Mind( int bot_, InputStream* istream ) : bot( bot_ )
  {
    flags = istream->readInt();

    lua.registerMind( bot );
  }

  Mind::~Mind()
  {
    lua.unregisterMind( bot );
  }

  void Mind::update()
  {
    hard_assert( orbis.objects[bot] != null );
    hard_assert( orbis.objects[bot]->flags & Object::BOT_BIT );

    Bot* bot = static_cast<Bot*>( orbis.objects[this->bot] );

    if( !bot->mindFunc.isEmpty() && !( bot->state & Bot::DEAD_BIT ) ) {
      const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

      flags &= ~FORCE_UPDATE_BIT;
      bot->actions = 0;
      lua.mindCall( clazz->mindFunction, bot );

      if( lua.forceUpdate ) {
        flags |= FORCE_UPDATE_BIT;
      }
    }
  }

  void Mind::write( OutputStream* ostream ) const
  {
    ostream->writeInt( flags );
  }

}
}
