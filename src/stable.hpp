/*
 *  stable.hpp
 *
 *  Precompiled header.
 *  It should be included in all ".cpp" source files in the project. Do not include it via another
 *  header, it will not work in GCC and VC++.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "oz/oz.hpp"
#include "configuration.hpp"

// we want to use C++ wrapped C headers, not vanilla C ones that are included via SDL
#include <cctype>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// gettext
#include <libintl.h>

// include SDL
#define SDL_NO_COMPAT
#include <SDL/SDL.h>

// include OpenGL
#ifdef OZ_OPENGL3
# define GL_VERSION_1_2_DEPRECATED
# define GL_VERSION_1_3_DEPRECATED
# define GL_VERSION_1_4_DEPRECATED
# define GL_VERSION_1_5_DEPRECATED
# define GL_VERSION_2_0_DEPRECATED
# define GL_VERSION_2_1_DEPRECATED
# define GL_VERSION_3_0_DEPRECATED
# define GL_ARB_imaging_DEPRECATED
# define GL_ARB_framebuffer_object_DEPRECATED
#endif

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

// include OpenAL
#include <AL/al.h>
