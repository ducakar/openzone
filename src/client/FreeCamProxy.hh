/*
 *  FreeCamProxy.hh
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

#include "client/Proxy.hh"

namespace oz
{
namespace client
{

  class FreeCamProxy : public Proxy
  {
    private:

      static const float LOW_SPEED;
      static const float HIGH_SPEED;

      bool fastMove;

    public:

      virtual void begin();
      virtual void update();
      virtual void prepare();

      void init();

  };

}
}
