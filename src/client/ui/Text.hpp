/*
 *  Text.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/ui/Label.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  class Area;

  class Text
  {
    private:

      int    x;
      int    y;
      int    width;
      int    lines;

      TTF_Font* font;
      Label* labels;

      static char buffer[2048];

    public:

      explicit Text( int x, int y, int width, int lines, Font::Type font );

      void setText( const char* s, ... );
      void clear();

      void draw( const Area* area ) const;

  };

}
}
}
