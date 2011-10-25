/*
 *  common.cpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

/**
 * @file oz/common.cpp
 */

#include "common.hpp"

#include "System.hpp"

#ifndef NDEBUG

namespace oz
{

void _hardAssert( const char* message, const char* file, int line, const char* function )
{
  System::trap();
  System::abort( "Hard assertion `%s' failed at %s:%d: %s", message, file, line, function );
}

void _softAssert( const char* message, const char* file, int line, const char* function )
{
  System::trap();
  System::error( "Soft assertion `%s' failed at %s:%d: %s", message, file, line, function );
}

}

#endif
