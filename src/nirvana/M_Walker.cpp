/*
 *  M_Walker.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "M_Walker.h"

namespace oz
{

  void M_Walker::onUpdate()
  {
    assert( world.objects[botIndex]->flags & Object::BOT_BIT );

    Bot *bot = (Bot*) world.objects[botIndex];

    bot->keys = 0;
    bot->keys |= Bot::KEY_FORWARD;

    if( Math::rand() % 101 == 0 ) {
      bot->h += ( Math::frand() * 120.0f ) - 60.0f;
    }
    if( Math::rand() % 67 == 0 ) {
      bot->keys |= Bot::KEY_JUMP;
    }
  }

}
