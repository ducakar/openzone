/*
 *  BasicAudio.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "./stable.hpp"

#include "client/Audio.hpp"

namespace oz
{
namespace client
{

  class BasicAudio : public Audio
  {
    protected:

      static const int SND_FRICTING = Object::EVENT_FRICTING;

      explicit BasicAudio( const Object* obj ) : Audio( obj ) {}

    public:

      static Pool<BasicAudio> pool;

      static Audio* create( const Object* obj );

      void play( const Audio* parent );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
