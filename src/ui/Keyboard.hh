/*
 *  Keyboard.hh
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

namespace oz
{
namespace client
{
namespace ui
{

  class Keyboard
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
