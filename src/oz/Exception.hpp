/*
 *  Exception.hpp
 *
 *  Exception class
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "String.hpp"

namespace oz
{

  class Exception : public std::exception
  {
    public:

      String      message;
      const char* file;
      int         line;
      const char* function;

      explicit Exception( const String& message_, const char* file_, int line_,
                          const char* function_ ) throw();

      virtual ~Exception() throw();

      virtual const char* what() const throw();

  };

  /**
   * \def Exception
   *
   * Exception constructor wrapper that provides the current file and line.
   */
# define Exception( message ) \
  oz::Exception( message, __FILE__, __LINE__, __PRETTY_FUNCTION__ )

}
