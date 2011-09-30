/*
 *  InfoFrame.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/ui/Frame.hpp"
#include "client/ui/Text.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  class InfoFrame : public Frame
  {
    private:

      Text text;
      int  lastId;

    protected:

      virtual void onVisibilityChange();
      virtual bool onMouseOver();
      virtual void onDraw();

    public:

      InfoFrame();

  };

}
}
}
