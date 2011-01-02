/*
 *  common.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#ifdef OZ_WINDOWS

#include "client/common.hpp"

PFNGLACTIVETEXTUREPROC        glActiveTexture;
PFNGLCLIENTACTIVETEXTUREPROC  glClientActiveTexture;
PFNGLGENBUFFERSPROC           glGenBuffers;
PFNGLDELETEBUFFERSPROC        glDeleteBuffers;
PFNGLBINDBUFFERPROC           glBindBuffer;
PFNGLBUFFERDATAPROC           glBufferData;

#endif
