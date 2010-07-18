/*
 *  common.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#if defined( OZ_MINGW ) || defined( OZ_MSVC )

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
  reinterpret_cast<const void*>( ( index ) * sizeof( Type ) )

#define OZ_VBO_OFFSETOF( index, VertexStruct, member ) \
  reinterpret_cast<const void*> \
  ( ( index ) * sizeof( VertexStruct ) + offsetof( VertexStruct, member ) )
