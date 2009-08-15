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

  M_Walker::M_Walker( Bot *body_ )
  {
    body = body_;
  }

  void M_Walker::onUpdate()
  {
    body->keys = 0;
    body->keys |= Bot::KEY_FORWARD;

    if( Math::rand() % 101 == 0 ) {
      body->h += ( Math::frand() * 120.0f ) - 60.0f;
    }
    if( Math::rand() % 67 == 0 ) {
      body->keys |= Bot::KEY_JUMP;
    }
  }

}
