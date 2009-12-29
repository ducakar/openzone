/*
 *  Button.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "Area.h"

namespace oz
{
namespace client
{
namespace ui
{

  struct Button : public Area
  {
    public:

      typedef void Callback( Button* sender );

    private:

      bool      isHighlighted;
      bool      isClicked;
      Callback* callback;

    protected:

      virtual void onMouseEvent();
      virtual void onDraw();

    public:

      String   label;

      explicit Button( const char* label_, Callback* callback, int width, int height ) :
          Area( width, height ), isHighlighted( false ), isClicked( false ), callback( callback ),
          label( label_ )
      {}

      void setCallback( Callback* callback_ )
      {
        callback = callback_;
      }

  };

}
}
}
