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

  struct BotAudio : public BasicAudio
  {
    protected:

      BotAudio( const Object* obj ) : BasicAudio( obj ) {}

    public:

      static Audio* create( const Object* obj );

      void play( const Audio* parent );

  };

}
}
