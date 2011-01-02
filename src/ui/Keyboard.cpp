/*
 *  Keyboard.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "ui/Keyboard.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  Keyboard keyboard;

  void Keyboard::init()
  {
    aSet<char>( keys, 0, SDLK_LAST );
    aSet<char>( oldKeys, 0, SDLK_LAST );
    currKeys = reinterpret_cast<char*>( SDL_GetKeyState( null ) );
  }

  void Keyboard::prepare()
  {
    aCopy( oldKeys, keys, SDLK_LAST );
    aCopy( keys, currKeys, SDLK_LAST );
  }

}
}
}
