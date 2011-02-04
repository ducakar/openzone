/*
 *  Exception.cpp
 *
 *  SIGTRAP signalling Exception constructor implementation.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "Exception.hpp"

#include "StackTrace.hpp"

#undef Exception

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>

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

  Exception::Exception( const String& message_, const char* file_, int line_,
                        const char* function_ ) throw() :
      message( message_ ), file( file_ ), line( line_ ), function( function_ ),
      nFrames( 0 ), frames( null )
  {
    nFrames = StackTrace::get( &frames );

#ifndef NDEBUG
# if defined( OZ_MSVC )
    DebugBreak();
# elif defined( OZ_MINGW )
    signal( SIGABRT, SIG_IGN );
    raise( SIGABRT );
    signal( SIGABRT, SIG_DFL );
# else
    signal( SIGTRAP, SIG_IGN );
    raise( SIGTRAP );
    signal( SIGTRAP, SIG_DFL );
# endif
#endif
  }

  Exception::~Exception() throw()
  {
    free( frames );
  }

  const char* Exception::what() const throw()
  {
    return message;
  }

}
