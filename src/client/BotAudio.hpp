/*
 *  BotAudio.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/BasicAudio.hpp"

namespace oz
{
namespace client
{

  class BotAudio : public BasicAudio
  {
    protected:

      explicit BotAudio( const Object* obj );

    public:

      static Pool<BotAudio> pool;

      static Audio* create( const Object* obj );

      void play( const Audio* parent );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
