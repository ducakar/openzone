/*
 *  Button.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/ui/Area.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  class Button : public Area
  {
    public:

      typedef void Callback( Button* sender );

    protected:

      Label     label;
      Callback* callback;
      bool      isHighlighted;
      bool      isClicked;

      virtual bool onMouseEvent();
      virtual void onDraw();

    public:

      explicit Button( const char* text, Callback* callback, int width, int height );

      void setCallback( Callback* callback );

  };

}
}
}
