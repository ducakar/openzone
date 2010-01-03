/*
 *  RandomMind.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "precompiled.h"

#include "RandomMind.h"

#include "Lua.h"

namespace oz
{
namespace nirvana
{

  Pool<RandomMind> RandomMind::pool;

  Mind* RandomMind::create( int botIndex )
  {
    RandomMind* mind = new RandomMind( botIndex );
    return mind;
  }

  Mind* RandomMind::read( InputStream* istream )
  {
    RandomMind* mind = new RandomMind( istream->readInt() );
    return mind;
  }

  const char* RandomMind::type() const
  {
    return "Random";
  }

  void RandomMind::update()
  {
    assert( world.objects[botIndex] != null );
    assert( world.objects[botIndex]->flags & Object::BOT_BIT );

    Bot& bot = *static_cast<Bot*>( world.objects[botIndex] );

    bot.actions = 0;

    if( bot.state & Bot::DEATH_BIT ) {
      return;
    }

    bot.actions |= Bot::ACTION_FORWARD;

    if( Math::rand() % 101 == 0 ) {
      bot.h += ( Math::frand() * 120.0f ) - 60.0f;
    }
    if( Math::rand() % 253 == 0 ) {
      bot.actions |= Bot::ACTION_JUMP;
    }
    if( Math::rand() % 253 == 0 ) {
      bot.state ^= Bot::RUNNING_BIT;
    }
  }

}
}
