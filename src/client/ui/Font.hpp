/*
 *  Font.hpp
 *
 *  Bitmap font loader and print function
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

struct _TTF_Font;
typedef _TTF_Font TTF_Font;

namespace oz
{
namespace client
{
namespace ui
{

  class Font
  {
    friend class Area;
    friend void init( int screenX, int screenY );
    friend void free();

    public:

      enum Type
      {
        MONO,
        SANS,
        SMALL,
        LARGE,
        MAX
      };

      struct Info
      {
        const char* file;
        int height;
      };

      static const SDL_Colour SDL_COLOUR_WHITE;
      static const Info INFOS[MAX];

      TTF_Font* fonts[MAX];

      uint textTexId;

      Font();

      bool init();
      void free();

  };

  extern Font font;

}
}
}
