/*
 *  BotAudio.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "BasicAudio.h"

namespace oz
{
namespace client
{

  struct BotAudio : BasicAudio
  {
    protected:

      BotAudio( const Object* obj ) : BasicAudio( obj ) {}

    public:

      static Pool<BotAudio> pool;

      static Audio* create( const Object* obj );

      void play( const Audio* parent );

    OZ_STATIC_POOL_ALLOC( pool );

  };

}
}
