/*
 *  Keyboard.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

namespace oz
{
namespace client
{
namespace ui
{

  struct Keyboard
  {
    private:

      ubyte* currKeys;

    public:

      ubyte  keys[SDLK_LAST];
      ubyte  oldKeys[SDLK_LAST];

      void init();
      void prepare();

  };

  extern Keyboard keyboard;

}
}
}
