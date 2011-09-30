/*
 *  Label.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/ui/Font.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  class Area;

  class Label
  {
    private:

      int        x;
      int        y;
      int        align;
      Font::Type font;

      int        offsetX;
      int        offsetY;
      int        width;
      int        height;

      uint       texId;
      uint       activeTexId;

    public:

      Label();
      ~Label();

      explicit Label( int x, int y, int align, Font::Type font, const char* s, ... );

      void vset( int x, int y, int align, Font::Type font, const char* s, va_list ap );
      void set( int x, int y, int align, Font::Type font, const char* s, ... );
      void setText( const char* s, ... );

      void draw( const Area* area ) const;

  };

}
}
}
