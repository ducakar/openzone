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

#include <cstdio>

#if defined( __native_client__ )
# include <ctime>
# include <sys/time.h>
#elif defined( _WIN32 )
# include <windows.h>
# include <mmsystem.h>
#else
# include <ctime>
#endif

namespace oz
{

#ifdef _WIN32

// Ensure that multimedia timer is set to 1 ms resolution during static initialisation.
static struct MediaTimerInitialiser
{
  OZ_HIDDEN
  MediaTimerInitialiser()
  {
    timeBeginPeriod( 1 );
  }
}
mediaTimerInitialiser;

#endif

uint Time::clock()
{
#if defined( __native_client__ )

  struct timeval now;
  gettimeofday( &now, nullptr );

  // This wraps around together with uint since (time_t range) * 1000 is a multiple of uint range.
  return uint( now.tv_sec * 1000 + now.tv_usec / 1000 );

#elif defined( _WIN32 )

  return timeGetTime();

#else

  struct timespec now;
  clock_gettime( CLOCK_MONOTONIC, &now );

  // This wraps around together with uint since (time_t range) * 1000 is a multiple of uint range.
  return uint( now.tv_sec * 1000 + now.tv_nsec / 1000000 );

#endif
}

uint Time::uclock()
{
#if defined( __native_client__ )

  struct timeval now;
  gettimeofday( &now, nullptr );

  // This wraps around together with uint since (time_t range) * 1000 is a multiple of uint range.
  return uint( now.tv_sec * 1000000 + now.tv_usec );

#elif defined( _WIN32 )

  return timeGetTime() * 1000;

#else

  struct timespec now;
  clock_gettime( CLOCK_MONOTONIC, &now );

  // This wraps around together with uint since (time_t range) * 10^6 is a multiple of uint range.
  return uint( now.tv_sec * 1000000 + now.tv_nsec / 1000 );

#endif
}

void Time::sleep( uint milliseconds )
{
#ifdef _WIN32

  Sleep( milliseconds );

#else

  struct timespec ts = {
    time_t( milliseconds / 1000 ),
    long( ( milliseconds % 1000 ) * 1000000 )
  };
  nanosleep( &ts, nullptr );

#endif
}

void Time::usleep( uint microseconds )
{
#ifdef _WIN32

  // Adding a millisecond rather rounding to the nearest millisecond value gives the most accurate
  // sleep periods for the given microsecond value. This conclusion is based on tests on Windows 7.
  Sleep( microseconds / 1000 + 1 );

#else

  struct timespec ts = {
    time_t( microseconds / 1000000 ),
    long( ( microseconds % 1000000 ) * 1000 )
  };
  nanosleep( &ts, nullptr );

#endif
}

long64 Time::time()
{
#ifdef _WIN32

  SYSTEMTIME     timeStruct;
  FILETIME       fileTime;
  ULARGE_INTEGER largeInteger;

  GetSystemTime( &timeStruct );
  SystemTimeToFileTime( &timeStruct, &fileTime );

  largeInteger.LowPart  = fileTime.dwLowDateTime;
  largeInteger.HighPart = fileTime.dwHighDateTime;

  return long64( largeInteger.QuadPart / 10000 );

#else

  return ::time( nullptr );

#endif
}

Time Time::local()
{
#ifdef _WIN32

  SYSTEMTIME     timeStruct;
  FILETIME       fileTime;
  ULARGE_INTEGER largeInteger;

  GetLocalTime( &timeStruct );
  SystemTimeToFileTime( &timeStruct, &fileTime );

  largeInteger.LowPart  = fileTime.dwLowDateTime;
  largeInteger.HighPart = fileTime.dwHighDateTime;

  return {
    long64( largeInteger.QuadPart / 10000 ),
    int( timeStruct.wYear ), int( timeStruct.wMonth ), int( timeStruct.wDay ),
    int( timeStruct.wHour ), int( timeStruct.wMinute ), int( timeStruct.wSecond )
  };

#else

  time_t currentTime = ::time( nullptr );
  struct tm timeStruct;
  localtime_r( &currentTime, &timeStruct );

  return {
    long64( currentTime ),
    int( 1900 + timeStruct.tm_year ), int( 1 + timeStruct.tm_mon ), int( timeStruct.tm_mday ),
    int( timeStruct.tm_hour ), int( timeStruct.tm_min ), int( timeStruct.tm_sec )
  };

#endif
}

Time Time::local( long64 epoch )
{
#ifdef _WIN32

  ULARGE_INTEGER largeInteger;
  FILETIME       fileTime;
  FILETIME       localFileTime;
  SYSTEMTIME     timeStruct;

  largeInteger.QuadPart = ulong64( epoch * 10000 );

  fileTime.dwLowDateTime  = largeInteger.LowPart;
  fileTime.dwHighDateTime = largeInteger.HighPart;

  FileTimeToLocalFileTime( &fileTime, &localFileTime );
  FileTimeToSystemTime( &localFileTime, &timeStruct );

  return {
    epoch,
    int( timeStruct.wYear ), int( timeStruct.wMonth ), int( timeStruct.wDay ),
    int( timeStruct.wHour ), int( timeStruct.wMinute ), int( timeStruct.wSecond )
  };

#else

  time_t ctime = time_t( epoch );
  struct tm timeStruct;
  localtime_r( &ctime, &timeStruct );

  return {
    epoch,
    int( 1900 + timeStruct.tm_year ), int( 1 + timeStruct.tm_mon ), int( timeStruct.tm_mday ),
    int( timeStruct.tm_hour ), int( timeStruct.tm_min ), int( timeStruct.tm_sec )
  };

#endif
}

long64 Time::toEpoch() const
{
#ifdef _WIN32

  SYSTEMTIME     timeStruct;
  FILETIME       localFileTime;
  FILETIME       fileTime;
  ULARGE_INTEGER largeInteger;

  timeStruct.wYear         = ushort( year );
  timeStruct.wMonth        = ushort( month );
  timeStruct.wDay          = ushort( day );
  timeStruct.wHour         = ushort( hour );
  timeStruct.wMinute       = ushort( minute );
  timeStruct.wSecond       = ushort( second );
  timeStruct.wMilliseconds = 0;

  SystemTimeToFileTime( &timeStruct, &localFileTime );
  LocalFileTimeToFileTime( &localFileTime, &fileTime );

  largeInteger.LowPart  = fileTime.dwLowDateTime;
  largeInteger.HighPart = fileTime.dwHighDateTime;

  return long64( largeInteger.QuadPart / 10000 );

#else

  struct tm timeStruct;

  timeStruct.tm_sec   = second;
  timeStruct.tm_min   = minute;
  timeStruct.tm_hour  = hour;
  timeStruct.tm_mday  = day;
  timeStruct.tm_mon   = month - 1;
  timeStruct.tm_year  = year - 1900;
  timeStruct.tm_isdst = -1;

  return long64( mktime( &timeStruct ) );

#endif
}

String Time::toString() const
{
  char* buffer;
  String r = String::create( 19, &buffer );

  snprintf( buffer, 20, "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second );
  return r;
}

}
