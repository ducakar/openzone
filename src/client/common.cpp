/*
 *  common.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/common.hpp"

#include <GL/gl.h>

namespace oz
{
namespace client
{

  const TexCoord TexCoord::ZERO = TexCoord( 0.0f, 0.0f );

  void glBindTexture( uint texId )
  {
    static uint currentTexId = 0;

    if( currentTexId != texId ) {
      currentTexId = texId;
      ::glBindTexture( GL_TEXTURE_2D, texId );
    }
  }

}
}
