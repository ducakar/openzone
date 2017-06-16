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
