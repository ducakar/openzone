/*
 *  MainMenu.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/ui/Frame.hpp"
#include "client/ui/Button.hpp"
#include "client/ui/Text.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  class MainMenu : public Frame
  {
    public:

      static String autosaveFile;
      static String quicksaveFile;

      Text copyright;

      MainMenu();

      virtual void onDraw();

  };

}
}
}
