/*
 *  LoadingArea.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "ui/LoadingArea.hpp"

#include "client/Camera.hpp"

#include <GL/gl.h>

namespace oz
{
namespace client
{
namespace ui
{

  LoadingArea::LoadingArea() : Area( camera.width, camera.height )
  {
    setFont( Font::TITLE );
  }

  LoadingArea::~LoadingArea()
  {}

  void LoadingArea::onDraw()
  {
    glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
    fill( 0, 0, camera.width, camera.height );
    printCentred( camera.width / 2, camera.height / 4, "LOADING" );
  }

}
}
}
