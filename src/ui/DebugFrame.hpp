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

#include "ui/Frame.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  class DebugFrame : public Frame
  {
    protected:

      virtual void onDraw();

    public:

      DebugFrame();

  };

}
}
}

#endif
