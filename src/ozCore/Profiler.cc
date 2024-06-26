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

#include "Profiler.hh"

namespace oz
{

namespace
{

HashMap<String, Duration> profileTimes;

}

Profiler::CRange Profiler::crange() noexcept
{
  return CRange(profileTimes.cbegin(), profileTimes.cend());
}

void Profiler::add(const char* key, Duration duration)
{
  Duration* time = profileTimes.find(key);

  if (time == nullptr) {
    profileTimes.add(key, duration);
  }
  else {
    *time += duration;
  }
}

void Profiler::clear()
{
  profileTimes.clear();
  profileTimes.trim();
}

}
