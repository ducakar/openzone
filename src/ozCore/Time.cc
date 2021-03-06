/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

#include <ctime>

namespace oz
{

int64 Time::epoch()
{
  return ::time(nullptr);
}

int64 Time::toEpoch() const
{
  struct tm timeStruct;

  timeStruct.tm_sec   = second;
  timeStruct.tm_min   = minute;
  timeStruct.tm_hour  = hour;
  timeStruct.tm_mday  = day;
  timeStruct.tm_mon   = month - 1;
  timeStruct.tm_year  = year - 1900;
  timeStruct.tm_isdst = -1;

  return mktime(&timeStruct);
}

Time Time::utc()
{
  return local(epoch());
}

Time Time::utc(int64 epoch)
{
  time_t ctime = time_t(epoch);
  struct tm timeStruct;
#ifdef _WIN32
  gmtime_s(&timeStruct, &ctime);
#else
  gmtime_r(&ctime, &timeStruct);
#endif

  return {
    1900 + timeStruct.tm_year, 1 + timeStruct.tm_mon, timeStruct.tm_mday,
    timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec
  };
}

Time Time::local()
{
  return local(epoch());
}

Time Time::local(int64 epoch)
{
  time_t ctime = time_t(epoch);
  struct tm timeStruct;
#ifdef _WIN32
  localtime_s(&timeStruct, &ctime);
#else
  localtime_r(&ctime, &timeStruct);
#endif

  return {
    1900 + timeStruct.tm_year, 1 + timeStruct.tm_mon, timeStruct.tm_mday,
    timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec
  };
}

String Time::toString() const
{
  return String::format("%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
}

}
