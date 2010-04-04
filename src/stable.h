/*
 *  precompiled.h
 *
 *  Precompiled header.
 *  It should be included in all ".cpp" source files in the project. Do not include it via another
 *  header, it will not work in GCC and VC++.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "base/base.h"

// we want to use C++ wrapped C headers, not vanilla ones that are included via SDL
#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <cctype>
// include SDL as it is commonly used
#include <SDL.h>
