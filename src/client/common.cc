/*
 *  common.cc
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hh"

#if defined( OZ_MINGW ) || defined( OZ_MSVC )

#include "client/common.hh"

PFNGLACTIVETEXTUREPROC        glActiveTexture;
PFNGLCLIENTACTIVETEXTUREPROC  glClientActiveTexture;
PFNGLGENBUFFERSPROC           glGenBuffers;
PFNGLDELETEBUFFERSPROC        glDeleteBuffers;
PFNGLBINDBUFFERPROC           glBindBuffer;
PFNGLBUFFERDATAPROC           glBufferData;

#endif
