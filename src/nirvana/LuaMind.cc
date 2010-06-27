/*
 *  LuaMind.cc
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hh"

#include "nirvana/LuaMind.hh"

#include "nirvana/Lua.hh"
#include "matrix/Bot.hh"
#include "matrix/BotClass.hh"

namespace oz
{
namespace nirvana
{

  Pool<LuaMind> LuaMind::pool;

  Mind* LuaMind::create( int botIndex )
  {
    LuaMind* mind = new LuaMind( botIndex );
    return mind;
  }

  Mind* LuaMind::read( InputStream* istream )
  {
    LuaMind* mind = new LuaMind( istream->readInt() );
    return mind;
  }

  LuaMind::LuaMind( int botIndex_ ) : Mind( botIndex_ )
  {
    lua.registerMind( botIndex );
  }

  LuaMind::~LuaMind()
  {
    lua.unregisterMind( botIndex );
  }

  const char* LuaMind::type() const
  {
    return "Lua";
  }

  void LuaMind::update()
  {
    assert( world.objects[botIndex] != null );
    assert( world.objects[botIndex]->flags & Object::BOT_BIT );

    Bot* bot = static_cast<Bot*>( world.objects[botIndex] );

    if( ~bot->state & Bot::DEATH_BIT ) {
      const BotClass* clazz = static_cast<const BotClass*>( bot->type );

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
