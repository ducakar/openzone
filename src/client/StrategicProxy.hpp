/*
 *  StrategicInput.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/Proxy.hpp"

namespace oz
{
namespace client
{

  class StrategicProxy : public Proxy
  {
    private:

      static const float MIN_HEIGHT;
      static const float MAX_HEIGHT;
      static const float DEFAULT_HEIGHT;
      static const float FREE_LOW_SPEED;
      static const float FREE_HIGH_SPEED;
      static const float RTS_LOW_SPEED;
      static const float RTS_HIGH_SPEED;
      static const float ZOOM_FACTOR;

      float height;
      bool  isFree;
      bool  isFreeFast;
      bool  isRTSFast;

    public:

      virtual void begin();
      virtual void update();
      virtual void prepare();

      void init();

  };

}
}
