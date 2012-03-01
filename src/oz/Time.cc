/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
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
