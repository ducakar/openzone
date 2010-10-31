/*
 *  Exception.cpp
 *
 *  SIGTRAP signalling Exception constructor implementation.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "Exception.hpp"

#ifndef NDEBUG

#undef Exception

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

  Exception::Exception( const String& message_, const char* file_, int line_,
                        const char* function_ ) throw() :
          message( message_ ), file( file_ ), line( line_ ), function( function_ )
  {
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
