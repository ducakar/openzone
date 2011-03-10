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

// we want to use C++ wrapped C headers, not vanilla ones that are included via SDL
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cstring>
#include <cinttypes>
// include SDL as it is commonly used
// #define SDL_NO_COMPAT
#include <SDL.h>
