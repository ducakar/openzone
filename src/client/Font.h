/*
 *  Font.h
 *
 *  Bitmap font loader and print function
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#define FONT_WIDTH      16.0f
#define FONT_HEIGHT     16.0f

namespace oz
{
namespace client
{

  class Font
  {
    private:

      GLuint texture;
      GLuint baseList;

      float  screenX;
      float  screenY;

      uint loadTexture( const char *fileName );

    public:

      Font();
      ~Font();

      void init( const char *fileName, float scale, float screenX, float screenY );
      void print( float x, float y, const char *string, ... ) const;
      void free();

  };

}
}
