/*
 *  SettingsFrame.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "client/ui/Frame.hpp"
#include "client/ui/Button.hpp"
#include "client/ui/Text.hpp"

namespace oz
{
namespace client
{
namespace ui
{

class SettingsFrame : public Frame
{
  private:

    Text    message;
    Button* close;

  protected:

    virtual void onDraw();

  public:

    SettingsFrame();

};

}
}
}
