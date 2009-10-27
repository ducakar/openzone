/*
 *  Exception.h
 *
 *  Exception class
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  struct Exception
  {
    const char *message;
    const char *file;
    int        line;

    explicit Exception( const char *message_, const char *file_, int line_ ) :
        message( message_ ), file( file_ ), line( line_ )
    {}
  };

  /**
   * \def Exception
   *
   * Exception constructor wrapper that provides the current file and line.
   */
# define Exception( message ) \
  oz::Exception( message, __FILE__, __LINE__ )

}
