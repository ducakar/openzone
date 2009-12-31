/*
 *  StrategicInput.h
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

  struct StrategicProxy : Proxy
  {
    private:

      static const float MIN_HEIGHT;
      static const float MAX_HEIGHT;
      static const float DEFAULT_HEIGHT;
      static const float LOW_SPEED;
      static const float HIGH_SPEED;
      static const float ZOOM_FACTOR;

      float height;
      bool  fastMove;

    public:

      virtual void begin();
      virtual void update();
      virtual void prepare();

  };

}
}
