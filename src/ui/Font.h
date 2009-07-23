/*
 *  Font.h
 *
 *  Bitmap font loader and print function
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{
namespace client
{
namespace ui
{

  enum FontType
  {
    MONO,
    SANS
  };

  class Font
  {
    friend class Area;

    friend void init( int screenX, int screenY );
    friend void free();

    public:

      TTF_Font *monoFont;
      TTF_Font *sansFont;

      int monoHeight;
      int sansHeight;

      bool init();
      void free();

  };

  extern Font font;

}
}
}
