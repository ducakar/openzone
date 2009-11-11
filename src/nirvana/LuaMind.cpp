/*
 *  LuaMind.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "LuaMind.h"

#include "Lua.h"
#include "matrix/Bot.h"
#include "matrix/BotClass.h"

namespace oz
{
namespace nirvana
{

  Mind *LuaMind::create( int botIndex )
  {
    LuaMind *mind = new LuaMind( botIndex );
    return mind;
  }

  Mind *LuaMind::read( InputStream *istream )
  {
    LuaMind *mind = new LuaMind( istream->readInt() );
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

  const char *LuaMind::type() const
  {
    return "Lua";
  }

  void LuaMind::update()
  {
    assert( world.objects[botIndex] != null );
    assert( world.objects[botIndex]->flags & Object::BOT_BIT );

    Bot *bot = static_cast<Bot*>( world.objects[botIndex] );

    if( ~bot->state & Bot::DEATH_BIT ) {
      BotClass *clazz = static_cast<BotClass*>( bot->type );

      bot->actions = 0;
      lua.call( clazz->mindFunction, bot );
    }
  }

}
}
