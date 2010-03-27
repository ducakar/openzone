/*
 *  BotProxy.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "client/Proxy.h"

namespace oz
{
namespace client
{

  class BotProxy : public Proxy
  {
    private:

      // leave this much space between obstacle and camera, if camera is brought closer to the eyes
      // because of an obstacle
      static const float THIRD_PERSON_CLIP_DIST = 0.20f;
      static const float BOB_SUPPRESSION_COEF   = 0.80f;
      static const float TURN_SMOOTHING_COEF    = 0.60f;

      // how far behind the eyes the camera should be
      float externalDistFactor;

      float bobPhi;
      float bobTheta;
      float bobBias;

      bool  isExternal;
      bool  isFreelook;

    public:

      virtual void begin();
      virtual void update();
      virtual void prepare();

      void init();

  };

}
}
