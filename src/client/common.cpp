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

#ifdef OZ_WINDOWS

PFNGLACTIVETEXTUREPROC        glActiveTexture;
PFNGLCLIENTACTIVETEXTUREPROC  glClientActiveTexture;
PFNGLGENBUFFERSPROC           glGenBuffers;
PFNGLDELETEBUFFERSPROC        glDeleteBuffers;
PFNGLBINDBUFFERPROC           glBindBuffer;
PFNGLBUFFERDATAPROC           glBufferData;

#endif

namespace oz
{
namespace client
{

  const TexCoord TexCoord::ZERO = TexCoord( 0.0f, 0.0f );

}
}
