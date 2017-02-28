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

/**
 * @file ozCore/Atomic.hh
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Memory order.
 */
enum MemoryOrder
{
  ATOMIC_ACQUIRE = __ATOMIC_ACQUIRE,
  ATOMIC_ACQ_REL = __ATOMIC_ACQ_REL,
  ATOMIC_CONSUME = __ATOMIC_CONSUME,
  ATOMIC_RELAXED = __ATOMIC_RELAXED,
  ATOMIC_RELEASE = __ATOMIC_RELEASE,
  ATOMIC_SEQ_CST = __ATOMIC_SEQ_CST
};

/**
 * Wrapper class for atomic operations on an integral variable.
 *
 * This class is meant as a light-weight replacement for `std::atomic` and `std::atomic_flag`.
 *
 * @sa `oz::SpinLock`, `oz::Mutex`, `oz::Semaphore`, `oz::CallOnce`, `oz::Thread`
 */
template <typename Type>
class Atomic
{
public:

  Type value; ///< Value of the atomic variable.

public:

  /**
   * No initialisation.
   */
  Atomic() = default;

  /**
   * Initialise to a given value.
   */
  Atomic(const Type& initialValue)
    : value(initialValue)
  {}

  /**
   * Atomically read the value and apply a memory barrier.
   *
   * Same as `std::atomic::load()`.
   */
  Type load(MemoryOrder memoryOrder)
  {
    return __atomic_load_n(&value, memoryOrder);
  }

  /**
   * Atomically write the value and apply a memory barrier.
   *
   * Same as `std::atomic::store()`.
   */
  void store(const Type& newValue, MemoryOrder memoryOrder)
  {
    __atomic_store_n(&value, newValue, memoryOrder);
  }

  /**
   * Atomically increment the value and apply a memory barrier.
   *
   * Same as `std::atomic::fetch_add()`.
   */
  void fetchAdd(const Type& increment, MemoryOrder memoryOrder)
  {
    __atomic_fetch_add(&value, increment, memoryOrder);
  }

  /**
   * Atomically set a new value and return the previous one and apply a memory barrier.
   *
   * Same as `std::atomic_flag::test_and_set()`. Only works for bool type.
   */
  bool testAndSet(MemoryOrder memoryOrder)
  {
    return __atomic_test_and_set(&value, memoryOrder);
  }

  /**
   * Atomically set the value to false and apply a memory barrier.
   *
   * Same as `std::atomic_flag::clear()`. Only works for bool type.
   */
  void clear(MemoryOrder memoryOrder)
  {
    __atomic_clear(&value, memoryOrder);
  }

};

}
