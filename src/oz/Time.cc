/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file oz/Time.cc
 */

#include "Time.hh"

#include "System.hh"

#ifdef _WIN32
# include "windefs.h"
# include <windows.h>
# include <mmsystem.h>
#else
# include <ctime>
# include <unistd.h>
#endif

namespace oz
{

#ifdef _WIN32

// The following struct is used to initialise and Windows high-resolution timer.
struct PerformanceTimer
{
  ulong64 resolution;
  ulong64 uresolution;

  PerformanceTimer()
  {
    timeBeginPeriod( 1 );

    LARGE_INTEGER frequency;
    if( QueryPerformanceFrequency( &frequency ) == 0 || frequency.QuadPart == 0 ) {
      System::error( 0, "PANIC: High-performance timer initialisation failed" );
    }

    resolution = ( 1000 + frequency.QuadPart / 2 ) / frequency.QuadPart;
    uresolution = ( 1000000 + frequency.QuadPart / 2 ) / frequency.QuadPart;
  }
};

PerformanceTimer performanceTimer;

#endif

uint Time::clock()
{
#ifdef _WIN32

  LARGE_INTEGER now;
  QueryPerformanceCounter( &now );

  return uint( now.QuadPart * performanceTimer.resolution );

#else

  struct timespec now;
  clock_gettime( CLOCK_MONOTONIC, &now );

  // This wraps around together with uint since (time_t range) * 1000 is a multiple of uint range.
  return uint( now.tv_sec * 1000 + now.tv_nsec / 1000000 );

#endif
}

void Time::sleep( uint milliseconds )
{
#ifdef _WIN32
  Sleep( milliseconds );
#else
  ::usleep( milliseconds * 1000 );
#endif
}

uint Time::uclock()
{
#ifdef _WIN32

  LARGE_INTEGER now;
  QueryPerformanceCounter( &now );

  return uint( now.QuadPart * performanceTimer.uresolution );

#else

  struct timespec now;
  clock_gettime( CLOCK_MONOTONIC, &now );

  // This wraps around together with uint since (time_t range) * 10^6 is a multiple of uint range.
  return uint( now.tv_sec * 1000000 + now.tv_nsec / 1000 );

#endif
}

void Time::usleep( uint microseconds )
{
#ifdef _WIN32
  Sleep( max<uint>( ( microseconds + 500 ) / 1000, 1 ) );
#else
  ::usleep( microseconds );
#endif
}

Time Time::utc()
{
#ifdef _WIN32

  SYSTEMTIME timeStruct;
  GetSystemTime( &timeStruct );

  return {
    timeStruct.wYear, timeStruct.wMonth, timeStruct.wDay, timeStruct.wHour,
    timeStruct.wMinute, timeStruct.wSecond
  };

#else

  struct tm timeStruct;
  time_t currentTime = ::time( null );
  gmtime_r( &currentTime, &timeStruct );

  return {
    1900 + timeStruct.tm_year, 1 + timeStruct.tm_mon, timeStruct.tm_mday,
    timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec
  };

#endif
}

Time Time::local()
{
#ifdef _WIN32

  SYSTEMTIME timeStruct;
  GetLocalTime( &timeStruct );

  return {
    timeStruct.wYear, timeStruct.wMonth, timeStruct.wDay, timeStruct.wHour,
    timeStruct.wMinute, timeStruct.wSecond
  };

#else

  struct tm timeStruct;
  time_t currentTime = ::time( null );
  localtime_r( &currentTime, &timeStruct );

  return {
    1900 + timeStruct.tm_year, 1 + timeStruct.tm_mon, timeStruct.tm_mday,
    timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec
  };

#endif
}

}
