/*
 *  Font.hpp
 *
 *  Bitmap font loader and print function
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
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

      enum Type : int
      {
        MONO,
        SANS,
        TITLE
      };

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
