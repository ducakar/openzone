/*
 *  common.cpp
 *
 *  _softAssert helper function
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "oz.hpp"

#ifndef NDEBUG

#include <cstdio>
#include <csignal>

#ifdef OZ_MSVC
# include <windows.h>
#endif

namespace oz
{

#ifndef OZ_MSVC
  static void sigtrapHandler( int )
  {}
#endif

  void _softAssert( const char* message, const char* file, int line, const char* function )
  {
    fprintf( stderr, "\n%s:%d: %s: Soft assertion `%s' failed.\n", file, line, function, message );

#if defined( OZ_MSVC )
    DebugBreak();
#elif defined( OZ_MINGW )
    signal( SIGABRT, sigtrapHandler );
    raise( SIGABRT );
#else
    signal( SIGTRAP, sigtrapHandler );
    raise( SIGTRAP );
#endif
  }

}

#endif
