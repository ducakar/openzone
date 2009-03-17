/*
 *  precompiled.hpp
 *
 *  Precompiled header.
 *  It should be included in all ".cpp" source files in the project. Do not include it via another
 *  header, it will not work in GCC and VC++.
 *
 *  Copyright (C) 2002-2008, Davorin Učakar <davorin.ucakar@gmail.com>
 */

// defining NDEBUG disables assert macro
// #define NDEBUG

#define OZ_USE_REUSEALLOC
#define OZ_USE_POOLALLOC

#include "src/base/base.hpp"

// include SDL, OpenGL and OpenAL as those are commonly used
#ifdef WIN32
# include <SDL.h>
# include <SDL_image.h>
# include <SDL_opengl.h>
# include <AL/alut.h>
#else
# include <SDL/SDL.h>
# include <SDL/SDL_image.h>
# include <SDL/SDL_opengl.h>
# include <AL/alut.h>
#endif
