/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file oz/Exception.cc
 */

#include "Exception.hh"

#include "System.hh"

#include <cstdio>

#undef Exception

namespace oz
{

Exception::Exception( const char* file_, int line_, const char* function_,
                      const char* message_, ... ) throw() :
    file( file_ ), function( function_ ), line( line_ )
{
  System::trap();

  va_list ap;
  va_start( ap, message_ );
  vsnprintf( message, 256, message_, ap );
  va_end( ap );

  stackTrace = StackTrace::current();
}

const char* Exception::what() const throw()
{
  return message;
}

}
