/*
 *  stable.hh
 *
 *  Precompiled header.
 *  It should be included in all ".cc" source files in the project. Do not include it via another
 *  header, it will not work in GCC and VC++.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "oz/oz.hh"
#include "configuration.hh"

// we want to use C++ wrapped C headers, not vanilla ones that are included via SDL
#include <cstddef>
#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <cctype>
// include SDL as it is commonly used
#include <SDL.h>

#ifdef OZ_MSVC
# define WIN32_LEAN_AND_MEAN 1
# include <windows.h>
# undef WIN32_LEAN_AND_MEAN
# undef min
# undef max
#endif
