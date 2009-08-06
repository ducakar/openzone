/*
 *  Button.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Button.h"

namespace oz
{
namespace client
{
namespace ui
{

  void Button::onClick( int, int )
  {
    assert( false );
  }

  void Button::draw()
  {
    glColor4f( 0.5f, 0.0f, 0.5f, 0.6f );
    fill( 0, 0, width, height );
  }

}
}
}
