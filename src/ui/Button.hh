/*
 *  Button.hh
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

#include "ui/Area.hh"

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

    private:

      bool      isHighlighted;
      bool      isClicked;
      Callback* callback;

    protected:

      virtual bool onMouseEvent();
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
