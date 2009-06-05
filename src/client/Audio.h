/*
 *  Audio.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/Object.h"

namespace oz
{
namespace client
{

  class Audio
  {
    protected:

      static const int ENTER_FUNC_BIT = 0x01;
      static const int EXIT_FUNC_BIT = 0x02;
      static const int UPDATE_FUNC_BIT = 0x04;

      const Object *obj;

      void playSoundEvent( const Object::Event *event ) const;
      void playSoundEffect( const Object::Effect *effect ) const;

    public:

      Audio( const Object *obj_ ) : obj( obj_ )
      {}

      virtual ~Audio();

      virtual void update() = 0;

  };

}
}
