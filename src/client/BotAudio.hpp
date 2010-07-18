/*
 *  BotAudio.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "client/BasicAudio.hpp"

namespace oz
{
namespace client
{

  class BotAudio : public BasicAudio
  {
    protected:

      BotAudio( const Object* obj ) : BasicAudio( obj ) {}

    public:

      static Pool<BotAudio> pool;

      static Audio* create( const Object* obj );

      void play( const Audio* parent );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
