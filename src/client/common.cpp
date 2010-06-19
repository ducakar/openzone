/*
 *  common.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "stable.hpp"

#if defined( OZ_MINGW32 ) || defined( OZ_MSVC )

#include "client/common.hpp"

PFNGLACTIVETEXTUREPROC        glActiveTexture;;
PFNGLCLIENTACTIVETEXTUREPROC  glClientActiveTexture;
PFNGLGENBUFFERSPROC           glGenBuffers;
PFNGLDELETEBUFFERSPROC        glDeleteBuffers;
PFNGLBINDBUFFERPROC           glBindBuffer;
PFNGLBUFFERDATAPROC           glBufferData;

#endif
