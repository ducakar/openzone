/*
 *  LoadingArea.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ui/LoadingArea.hpp"

#include "client/Camera.hpp"
#include "client/Shader.hpp"

#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  LoadingArea::LoadingArea() : Area( camera.width, camera.height ),
      status( camera.width / 2, camera.height / 4, ALIGN_CENTRE, Font::TITLE, "" )
  {
    flags = PINNED_BIT;
  }

  LoadingArea::~LoadingArea()
  {}

  void LoadingArea::onDraw()
  {
    glUniform4f( param.oz_Colour, 0.0f, 0.0f, 0.0f, 1.0f );
    fill( 0, 0, camera.width, camera.height );

    status.draw( this );
  }

}
}
}
