/*
 *  Keyboard.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "./stable.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  class Keyboard
  {
    private:

      char* currKeys;

    public:

      char  keys[SDLK_LAST];
      char  oldKeys[SDLK_LAST];

      void init();
      void prepare();

  };

  extern Keyboard keyboard;

}
}
}
