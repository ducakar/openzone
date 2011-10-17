/*
 *  stable.hpp
 *
 *  Precompiled header.
 *  It should be included in all ".cpp" source files in the project. Do not include it via another
 *  header, it will not work in GCC or VC++.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "oz/oz.hpp"
#include "configuration.hpp"

// we want to use C++ wrapped C headers, not vanilla C ones that are included via SDL
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// gettext
#include <libintl.h>

// include SDL
#define SDL_NO_COMPAT
#include <SDL/SDL.h>

// fix M$ crap from Windows headers
#ifdef OZ_MINGW
# undef ERROR
# undef PLANES
# undef near
# undef far
#endif
