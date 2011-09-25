/*
 *  Exception.cpp
 *
 *  SIGTRAP signalling Exception constructor implementation.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "Exception.hpp"

#include "System.hpp"

#include <cstdlib>

#undef Exception

namespace oz
{

  Exception::Exception( const String& message_, const char* file_, int line_,
                        const char* function_ ) throw() :
      message( message_ ), file( file_ ), function( function_ ), line( line_ ),
      nFrames( 0 ), frames( null )
  {
    System::trap();
    nFrames = System::getStackTrace( &frames );
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
