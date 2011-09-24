/*
 *  DebugFrame.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#ifndef NDEBUG

#include "client/ui/Frame.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  class DebugFrame : public Frame
  {
    protected:

      Label camPosRot;
      Label botPosRot;
      Label botVelMom;
      Label botFlagsState;
      Label tagVelMom;
      Label tagFlags;

      virtual void onDraw();

    public:

      DebugFrame();

  };

}
}
}

#endif
