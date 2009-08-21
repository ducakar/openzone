/*
 *  Button.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Area.h"

namespace oz
{
namespace client
{
namespace ui
{

  class Button : public Area
  {
    public:

      typedef void Callback( Button *sender );

    private:

      String   label;
      bool     isHighlighted;
      bool     isClicked;
      Callback *callback;

    protected:

      virtual void onMouseEvent();
      virtual void draw();

    public:

      Button( const char *label_, Callback *callback, int width, int height ) :
          Area( width, height ), label( label_ ), isHighlighted( false ), isClicked( false ),
          callback( callback )
      {}

      void setCallback( Callback *callback_ )
      {
        callback = callback_;
      }

  };

}
}
}
