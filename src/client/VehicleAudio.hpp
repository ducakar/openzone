/*
 *  VehicleAudio.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/BasicAudio.hpp"

namespace oz
{
namespace client
{

  class VehicleAudio : public BasicAudio
  {
    protected:

      explicit VehicleAudio( const Object* obj ) : BasicAudio( obj )
      {}

    public:

      static Pool<VehicleAudio> pool;

      static Audio* create( const Object* obj );

      void play( const Audio* parent );

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
