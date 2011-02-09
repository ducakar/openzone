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

      int         nFrames;
      char*       frames;

      explicit Exception( const String& message, const char* file, int line,
                          const char* function ) throw();

      virtual ~Exception() throw();

      virtual const char* what() const throw();

  };

  /**
   * \def Exception
   * Exception constructor wrapper that provides the current file and line.
   */
# define Exception( message ) \
  oz::Exception( message, __FILE__, __LINE__, __PRETTY_FUNCTION__ )

  /**
   * Helper class for onleave macro.
   */
  template <typename Function>
  class _OnLeave
  {
    private:

      Function func;

    public:

      explicit _OnLeave( const Function& func_ ) : func( func_ )
      {}

      ~_OnLeave()
      {
        func();
      }

  };

  /**
   * \def OZ_ONLEAVE_CODE
   * Helper for onleave macro (used internally).
   */
#define _OZ_ONLEAVE_CODE( func, line ) \
  auto _onLeaveFunc_##line = func; \
  oz::_OnLeave< decltype( _onLeaveFunc_##line ) > _onLeaveGuard_##line( _onLeaveFunc_##line )

  /**
   * \def OZ_ONLEAVE_AUX
   * Auxilary helper for onleave macro (used internally).
   * This intermediate macro is required to instantiate __LINE__ macro for its value.
   */
#define _OZ_ONLEAVE_AUX( func, line ) \
  _OZ_ONLEAVE_CODE( func, line )

  /**
   * \def onleave
   * Macro that executes given (lambda) function when the current block of code exits.
   * Cleanup snippets are executed in the opposite order as they were defined inside a block.
   * This is the preferred way of cleaning up things for the case an exception is thrown.
   */
#define onleave( func ) \
  _OZ_ONLEAVE_AUX( func, __LINE__ )

}
