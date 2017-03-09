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

#include "System.hh"

namespace oz
{

/**
 * Memory order.
 */
enum MemoryOrder
{
  ATOMIC_RELAXED = __ATOMIC_RELAXED,
  ATOMIC_CONSUME = __ATOMIC_CONSUME,
  ATOMIC_ACQUIRE = __ATOMIC_ACQUIRE,
  ATOMIC_RELEASE = __ATOMIC_RELEASE,
  ATOMIC_ACQ_REL = __ATOMIC_ACQ_REL,
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

  Type value; ///< Value of the atomic variable (for non-atomic access, without memory barriers).

public:

  /**
   * Apply a memory barrier.
   */
  template <MemoryOrder MEMORY_ORDER>
  static void threadFence()
  {
    __atomic_thread_fence(MEMORY_ORDER);
  }

  /**
   * Atomically set a new value and return the previous value.
   *
   * Same as `std::atomic_flag::test_and_set()`. Only works for bool type.
   */
  template <MemoryOrder MEMORY_ORDER>
  bool testAndSet()
  {
    return __atomic_test_and_set(&value, MEMORY_ORDER);
  }

  /**
   * Atomically set the value to false.
   *
   * Same as `std::atomic_flag::clear()`. Only works for bool type. Only `ATOMIC_RELAXED`,
   * `ATOMIC_RELEASE` and `ATOMIC_SEQ_CST` memory orders are allowed.
   */
  template <MemoryOrder MEMORY_ORDER>
  void clear()
  {
    static_assert(MEMORY_ORDER == ATOMIC_RELAXED ||
                  MEMORY_ORDER == ATOMIC_RELEASE ||
                  MEMORY_ORDER == ATOMIC_SEQ_CST,
                  "Unsupported memory order");

    __atomic_clear(&value, MEMORY_ORDER);
  }

  /**
   * Atomically read the value.
   *
   * Same as `std::atomic::load()`. Only `ATOMIC_RELAXED`, `ATOMIC_ACQUIRE`, `ATOMIC_CONSUME` and
   * `ATOMIC_SEQ_CST` memory orders are allowed.
   */
  template <MemoryOrder MEMORY_ORDER>
  Type load() const
  {
    static_assert(MEMORY_ORDER == ATOMIC_RELAXED ||
                  MEMORY_ORDER == ATOMIC_CONSUME ||
                  MEMORY_ORDER == ATOMIC_ACQUIRE ||
                  MEMORY_ORDER == ATOMIC_SEQ_CST,
                  "Unsupported memory order");

    return __atomic_load_n(&value, MEMORY_ORDER);
  }

  /**
   * Atomically write a value.
   *
   * Same as `std::atomic::store()`. Only `ATOMIC_RELAXED`, `ATOMIC_RELEASE` and `ATOMIC_SEQ_CST`
   * memory orders are allowed.
   */
  template <MemoryOrder MEMORY_ORDER>
  void store(Type desired)
  {
    static_assert(MEMORY_ORDER == ATOMIC_RELAXED ||
                  MEMORY_ORDER == ATOMIC_RELEASE ||
                  MEMORY_ORDER == ATOMIC_SEQ_CST,
                  "Unsupported memory order");

    __atomic_store_n(&value, desired, MEMORY_ORDER);
  }

  /**
   * Atomically write a value and return the old one.
   *
   * Same as `std::atomic::exchange()`.
   */
  template <MemoryOrder MEMORY_ORDER>
  Type exchange(Type desired)
  {
    static_assert(MEMORY_ORDER == ATOMIC_RELAXED ||
                  MEMORY_ORDER == ATOMIC_ACQUIRE ||
                  MEMORY_ORDER == ATOMIC_RELEASE ||
                  MEMORY_ORDER == ATOMIC_ACQ_REL ||
                  MEMORY_ORDER == ATOMIC_SEQ_CST,
                  "Unsupported memory order");

    return __atomic_exchange_n(&value, desired, MEMORY_ORDER);
  }

  /**
   * Atomically perfrom bitwise AND and return the previous value.
   *
   * Same as `std::atomic::fetch_and()`.
   */
  template <MemoryOrder MEMORY_ORDER>
  Type fetchAnd(Type arg)
  {
    return __atomic_fetch_and(&value, arg, MEMORY_ORDER);
  }

  /**
   * Atomically perfrom bitwise OR and return the previous value.
   *
   * Same as `std::atomic::fetch_or()`.
   */
  template <MemoryOrder MEMORY_ORDER>
  Type fetchOr(Type arg)
  {
    return __atomic_fetch_or(&value, arg, MEMORY_ORDER);
  }

  /**
   * Atomically perfrom bitwise XOR and return the previous value.
   *
   * Same as `std::atomic::fetch_xor()`.
   */
  template <MemoryOrder MEMORY_ORDER>
  Type fetchXor(Type arg)
  {
    return __atomic_fetch_xor(&value, arg, MEMORY_ORDER);
  }

  /**
   * Atomically increment the value and return the previous value.
   *
   * Same as `std::atomic::fetch_add()`.
   */
  template <MemoryOrder MEMORY_ORDER>
  Type fetchAdd(Type arg)
  {
    return __atomic_fetch_add(&value, arg, MEMORY_ORDER);
  }

  /**
   * Atomically decrement the value and return the previous value.
   *
   * Same as `std::atomic::fetch_add()`.
   */
  template <MemoryOrder MEMORY_ORDER>
  Type fetchSub(Type arg)
  {
    return __atomic_fetch_sub(&value, arg, MEMORY_ORDER);
  }

};

}
