/*
 *  LuaMind.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
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

  Mind* LuaMind::create( int iBot )
  {
    LuaMind* mind = new LuaMind( iBot );
    return mind;
  }

  Mind* LuaMind::read( InputStream* istream )
  {
    LuaMind* mind = new LuaMind( istream->readInt() );
    return mind;
  }

  LuaMind::LuaMind( int iBot_ ) : Mind( iBot_ )
  {
    lua.registerMind( iBot );
  }

  LuaMind::~LuaMind()
  {
    lua.unregisterMind( iBot );
  }

  const char* LuaMind::type() const
  {
    return "Lua";
  }

  void LuaMind::update()
  {
    assert( orbis.objects[iBot] != null );
    assert( orbis.objects[iBot]->flags & Object::BOT_BIT );

    Bot* bot = static_cast<Bot*>( orbis.objects[iBot] );

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
