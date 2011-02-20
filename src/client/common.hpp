/*
 *  common.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Collider.hpp"

#ifdef OZ_WINDOWS

# include <SDL_opengl.h>

extern PFNGLACTIVETEXTUREPROC        glActiveTexture;
extern PFNGLCLIENTACTIVETEXTUREPROC  glClientActiveTexture;
extern PFNGLGENBUFFERSPROC           glGenBuffers;
extern PFNGLDELETEBUFFERSPROC        glDeleteBuffers;
extern PFNGLBINDBUFFERPROC           glBindBuffer;
extern PFNGLBUFFERDATAPROC           glBufferData;

#else

# define GL_GLEXT_PROTOTYPES

#endif

#define OZ_VBO_OFFSET( index, Type ) \
  ( reinterpret_cast<const char*>( 0 ) + ( index ) * sizeof( Type ) )

#define OZ_VBO_OFFSETOF( index, VertexStruct, member ) \
  ( reinterpret_cast<const char*>( 0 ) + ( index ) * sizeof( VertexStruct ) + \
      offsetof( VertexStruct, member ) )

namespace oz
{
namespace client
{

  extern Collider collider;

}
}
