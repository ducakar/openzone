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

/**
 * @file ozCore/Profiler.hh
 *
 * `Profiler` class.
 */

#pragma once

#include "HashMap.hh"
#include "String.hh"
#include "Duration.hh"

/**
 * Remember current time for a later `OZ_PROFILER_END()` macro with the same key.
 */
#define OZ_PROFILER_BEGIN(key) \
  Instant key##_profilerInstant = oz::Instant<STEADY>::now()

/**
 * Enter time difference into the `oz::Profiler` under a given key.
 *
 * Time difference is calculated as difference between the current time and the saved time from a
 * previous `oz_PROFILER_BEGIN()` with the same key.
 */
#define OZ_PROFILER_END(key) \
  oz::Profiler::add(#key, oz::Instant<STEADY>::now() - key##_profilerInstant)

namespace oz
{

/**
 * Profiling statistics.
 *
 * This class has an internal hashtable of string-microseconds pairs. `add()` function may be used
 * to add a new entry or add time to an existing entry, but `OZ_PROFILER_BEGIN()` and
 * `OZ_PROFILER_END()` macros are probably better suited.
 */
class Profiler
{
public:

  /**
   * Constant iterator for accumulated times.
   */
  using CRange = HashMap<String, Duration>::CRange;

public:

  /**
   * Return constant iterator for accumulated times.
   */
  static CRange crange() noexcept;

  /**
   * Add a time to a named sum.
   *
   * If the key doesn't exist, a new key with initial value `duration` is added.
   */
  static void add(const char* key, Duration duration);

  /**
   * Clear internal hashtable, deleting all times.
   */
  static void clear();

};

}
