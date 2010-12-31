/*
 *  Font.hpp
 *
 *  Bitmap font loader and print function
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include <SDL_ttf.h>

namespace oz
{
namespace client
{
namespace ui
{

  enum FontType
  {
    MONO,
    SANS,
    TITLE
  };

  class Font
  {
    friend class Area;

    friend void init( int screenX, int screenY );
    friend void free();

    public:

      TTF_Font* monoFont;
      TTF_Font* sansFont;
      TTF_Font* titleFont;

      int monoHeight;
      int sansHeight;
      int titleHeight;

      bool init();
      void free();

  };

  extern Font font;

}
}
}
