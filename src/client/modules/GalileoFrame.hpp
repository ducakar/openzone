/*
 *  GalileoFrame.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/ui/Frame.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  class GalileoFrame : public Frame
  {
    protected:

      virtual void onDraw();

    public:

      GalileoFrame();

  };

}
}
}
