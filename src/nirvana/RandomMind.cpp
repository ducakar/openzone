/*
 *  RandomMind.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "nirvana/RandomMind.hpp"

#include "nirvana/Lua.hpp"

namespace oz
{
namespace nirvana
{

  Pool<RandomMind> RandomMind::pool;

  Mind* RandomMind::create( int bot )
  {
    RandomMind* mind = new RandomMind( bot );
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
    hard_assert( orbis.objects[bot] != null );
    hard_assert( orbis.objects[bot]->flags & Object::BOT_BIT );

    Bot& bot = *static_cast<Bot*>( orbis.objects[this->bot] );

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
