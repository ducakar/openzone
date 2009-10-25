/*
 *  RandomMind.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "RandomMind.h"

namespace oz
{

  Mind *RandomMind::create( int botIndex )
  {
    RandomMind *mind = new RandomMind( botIndex );
    return mind;
  }

  Mind *RandomMind::read( InputStream *istream )
  {
    RandomMind *mind = new RandomMind( istream->readInt() );
    return mind;
  }

  const char *RandomMind::type() const
  {
    return "Random";
  }

  void RandomMind::update()
  {
    assert( world.objects[botIndex] != null );
    assert( world.objects[botIndex]->flags & Object::BOT_BIT );

    Bot &bot = *(Bot*) world.objects[botIndex];

    bot.keys = 0;
    bot.keys |= Bot::KEY_FORWARD;

    if( Math::rand() % 101 == 0 ) {
      bot.h += ( Math::frand() * 120.0f ) - 60.0f;
    }
    if( Math::rand() % 253 == 0 ) {
      bot.keys |= Bot::KEY_JUMP;
    }
  }

}
