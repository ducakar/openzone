/*
 *  FreeCamProxy.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "Proxy.h"

namespace oz
{
namespace client
{

  struct FreeCamProxy : Proxy
  {
    private:

      static const float LOW_SPEED;
      static const float HIGH_SPEED;

      bool fastMove;

    public:

      virtual void begin();
      virtual void update();
      virtual void prepare();

  };

}
}
