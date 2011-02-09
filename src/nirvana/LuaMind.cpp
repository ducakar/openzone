/*
 *  LuaMind.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "nirvana/LuaMind.hpp"

#include "nirvana/Lua.hpp"
#include "matrix/Bot.hpp"
#include "matrix/BotClass.hpp"

namespace oz
{
namespace nirvana
{

  Pool<LuaMind> LuaMind::pool;

  Mind* LuaMind::create( int bot )
  {
    LuaMind* mind = new LuaMind( bot );
    return mind;
  }

  Mind* LuaMind::read( InputStream* istream )
  {
    LuaMind* mind = new LuaMind( istream->readInt() );
    return mind;
  }

  LuaMind::LuaMind( int bot_ ) : Mind( bot_ )
  {
    lua.registerMind( bot );
  }

  LuaMind::~LuaMind()
  {
    lua.unregisterMind( bot );
  }

  const char* LuaMind::type() const
  {
    return "Lua";
  }

  void LuaMind::update()
  {
    hard_assert( orbis.objects[bot] != null );
    hard_assert( orbis.objects[bot]->flags & Object::BOT_BIT );

    Bot* bot = static_cast<Bot*>( orbis.objects[this->bot] );

    if( !( bot->state & Bot::DEATH_BIT ) ) {
      const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

      flags &= ~FORCE_UPDATE_BIT;
      bot->actions = 0;
      lua.call( clazz->mindFunction, bot );

      if( lua.forceUpdate ) {
        flags |= FORCE_UPDATE_BIT;
      }
    }
  }

}
}
