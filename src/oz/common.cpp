/*
 *  common.cpp
 *
 *  _softAssert helper function
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "common.hpp"

#ifndef NDEBUG

#include <cstdio>
#include <csignal>

// prevent old-style cast warning due to a bug in <bits/signum.h>
#ifdef __GNUC__
# undef SIG_DFL
# undef SIG_IGN
# define SIG_DFL reinterpret_cast<__sighandler_t>( 0 )            /* Default action.  */
# define SIG_IGN reinterpret_cast<__sighandler_t>( 1 )            /* Ignore signal.  */
#endif

#ifdef OZ_MSVC
# include <windows.h>
#endif

namespace oz
{

  void _softAssert( const char* message, const char* file, int line, const char* function )
  {
    fprintf( stderr, "%s:%d: %s: Soft assertion `%s' failed.\n", file, line, function, message );

#if defined( OZ_MSVC )
    DebugBreak();
#elif defined( OZ_UNIX )
    signal( SIGTRAP, SIG_IGN );
    raise( SIGTRAP );
    signal( SIGTRAP, SIG_DFL );
#endif
  }

}

#endif
