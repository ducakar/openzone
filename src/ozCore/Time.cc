/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

#include "Time.hh"

#ifdef _WIN32
# include <windows.h>
#else
# include <ctime>
#endif

namespace oz
{

uint Time::clock()
{
#ifdef _WIN32

  LARGE_INTEGER frequency;
  LARGE_INTEGER ticks;

  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&ticks);

  // This is not continuous when performance counter wraps around.
  return uint((ticks.QuadPart * 1000) / frequency.QuadPart);

#else

  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);

  // This is continuous if tv_sec wraps around at its maximum value since (time_t range) * 1000 is
  // a multiple of uint range.
  return uint(now.tv_sec * 1000 + now.tv_nsec / 1000000);

#endif
}

uint Time::uclock()
{
#ifdef _WIN32

  LARGE_INTEGER frequency;
  LARGE_INTEGER ticks;

  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&ticks);

  // This is not continuous when performance counter wraps around.
  return uint((ticks.QuadPart * 1000000) / frequency.QuadPart);

#else

  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);

  // This is continuous if tv_sec wraps around at its maximum value since (time_t range) * 1000000
  // is a multiple of uint range.
  return uint(now.tv_sec * 1000000 + now.tv_nsec / 1000);

#endif
}

void Time::sleep(uint milliseconds)
{
#ifdef _WIN32

  Sleep(milliseconds);

#else

  struct timespec ts = {
    time_t(milliseconds / 1000),
    long((milliseconds % 1000) * 1000000)
  };
# ifdef __native_client__
  nanosleep(&ts, nullptr);
# else
  clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, nullptr);
# endif

#endif
}

void Time::usleep(uint microseconds)
{
#ifdef _WIN32

  // Based on observations performed on Windows 7, adding a millisecond rather than rounding to the
  // nearest millisecond value gives the most accurate sleep periods for a given microsecond value.
  Sleep(microseconds / 1000 + 1);

#else

  struct timespec ts = {
    time_t(microseconds / 1000000),
    long((microseconds % 1000000) * 1000)
  };
# ifdef __native_client__
  nanosleep(&ts, nullptr);
# else
  clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, nullptr);
# endif

#endif
}

long64 Time::epoch()
{
#ifdef _WIN32

  SYSTEMTIME     timeStruct;
  FILETIME       fileTime;
  ULARGE_INTEGER largeInteger;

  GetSystemTime(&timeStruct);
  SystemTimeToFileTime(&timeStruct, &fileTime);

  largeInteger.LowPart  = fileTime.dwLowDateTime;
  largeInteger.HighPart = fileTime.dwHighDateTime;

  return largeInteger.QuadPart / 10000;

#else

  return ::time(nullptr);

#endif
}

long64 Time::toEpoch() const
{
#ifdef _WIN32

  SYSTEMTIME     timeStruct;
  FILETIME       localFileTime;
  FILETIME       fileTime;
  ULARGE_INTEGER largeInteger;

  timeStruct.wYear         = ushort(year);
  timeStruct.wMonth        = ushort(month);
  timeStruct.wDay          = ushort(day);
  timeStruct.wHour         = ushort(hour);
  timeStruct.wMinute       = ushort(minute);
  timeStruct.wSecond       = ushort(second);
  timeStruct.wMilliseconds = 0;

  SystemTimeToFileTime(&timeStruct, &localFileTime);
  LocalFileTimeToFileTime(&localFileTime, &fileTime);

  largeInteger.LowPart  = fileTime.dwLowDateTime;
  largeInteger.HighPart = fileTime.dwHighDateTime;

  return largeInteger.QuadPart / 10000;

#else

  struct tm timeStruct;

  timeStruct.tm_sec   = second;
  timeStruct.tm_min   = minute;
  timeStruct.tm_hour  = hour;
  timeStruct.tm_mday  = day;
  timeStruct.tm_mon   = month - 1;
  timeStruct.tm_year  = year - 1900;
  timeStruct.tm_isdst = -1;

  return mktime(&timeStruct);

#endif
}

Time Time::local()
{
  return local(epoch());
}

Time Time::local(long64 epoch)
{
#ifdef _WIN32

  ULARGE_INTEGER largeInteger;
  FILETIME       fileTime;
  FILETIME       localFileTime;
  SYSTEMTIME     timeStruct;

  largeInteger.QuadPart = epoch * 10000;

  fileTime.dwLowDateTime  = largeInteger.LowPart;
  fileTime.dwHighDateTime = largeInteger.HighPart;

  FileTimeToLocalFileTime(&fileTime, &localFileTime);
  FileTimeToSystemTime(&localFileTime, &timeStruct);

  return {
    timeStruct.wYear, timeStruct.wMonth, timeStruct.wDay,
    timeStruct.wHour, timeStruct.wMinute, timeStruct.wSecond
  };

#else

  time_t ctime = time_t(epoch);
  struct tm timeStruct;
  localtime_r(&ctime, &timeStruct);

  return {
    1900 + timeStruct.tm_year, 1 + timeStruct.tm_mon, timeStruct.tm_mday,
    timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec
  };

#endif
}

String Time::toString() const
{
  return String::format("%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
}

}
