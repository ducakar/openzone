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

  class Font
  {
    public:

      enum Type
      {
        MONO,
        SANS
      };

    private:

      float     screenX;
      float     screenY;

      TTF_Font  *monoFont;
      int       monoHeight;
      TTF_Font  *sansFont;
      int       sansHeight;

      TTF_Font  *currentFont;

      SDL_Color fgColor;
      SDL_Color bgColor;

    public:

      void setFont( Type type )
      {
        currentFont = type == MONO ? monoFont : sansFont;
      }

      int getHeight() const
      {
        return currentFont == monoFont ? monoHeight : sansHeight;
      }

      void print( float x, float y, const char *string, ... );

      bool init( float screenX, float screenY );
      void free();

  };

  extern Font font;

}
}
}
