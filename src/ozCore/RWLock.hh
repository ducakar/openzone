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
 * @file ozCore/RWLock.hh
 *
 * `RWLock` class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Readers-writer lock / shared mutex.
 *
 * @note No error checks are performed on the underlying pthreads API so any abuse of this class
 * leads to undefined behaviour.
 *
 * @sa `oz::SpinLock`, `oz::Monitor`
 */
class RWLock
{
public:

  /**
   * Accessor for reader (shared) lock.
   */
  class Read
  {
    friend class RWLock;

  private:

    RWLock* const parent_; ///< Parent shared mutex.

  private:

    /**
     * Create reader lock accessor for a given shared mutex.
     */
    explicit Read(RWLock* parent)
      : parent_(parent)
    {}

  public:

    /**
     * No copying.
     */
    Read(const Read&) = delete;

    /**
     * No copying.
     */
    Read& operator=(const Read&) = delete;

    /**
     * Wait until reader lock is acquired.
     *
     * @note
     * Locking a mutex that is already locked by the current thread results in undefined behaviour.
     */
    void lock();

    /**
     * Acquire reader lock if not already locked.
     *
     * @note
     * Locking a mutex that is already locked by the current thread results in undefined behaviour.
     *
     * @return True on success.
     */
    bool tryLock();

    /**
     * Release reader lock.
     *
     * @note
     * Unlocking an unlocked mutex results in undefined behaviour.
     */
    void unlock();

  };

  /**
   * Accessor for writer (exclusive) lock.
   */
  class Write
  {
    friend class RWLock;

  private:

    RWLock* const parent_; ///< Parent shared mutex.

  private:

    /**
     * Create writer lock accessor for a given shared mutex.
     */
    explicit Write(RWLock* parent)
      : parent_(parent)
    {}

  public:

    /**
     * No copying.
     */
    Write(const Write&) = delete;

    /**
     * No copying.
     */
    Write& operator=(const Write&) = delete;

    /**
     * Wait until writer lock is acquired.
     *
     * @note
     * Locking a mutex that is already locked by the current thread results in undefined behaviour.
     */
    void lock();

    /**
     * Acquire writer lock if not already locked.
     *
     * @note
     * Locking a mutex that is already locked by the current thread results in undefined behaviour.
     *
     * @return True on success.
     */
    bool tryLock();

    /**
     * Release writer lock.
     *
     * @note
     * Unlocking an unlocked mutex results in undefined behaviour.
     */
    void unlock();

  };

  Read  read;  ///< Accessor for reader lock.
  Write write; ///< Accessor for writer lock.

private:

  struct Descriptor;

  Descriptor* descriptor_; ///< Internal shared mutex descriptor.

public:

  /**
   * Create and initialise shared mutex.
   */
  RWLock();

  /**
   * Destroy shared mutex.
   */
  ~RWLock();

  /**
   * Copying or moving is not possible.
   */
  RWLock(const RWLock&) = delete;

  /**
   * Copying or moving is not possible.
   */
  RWLock& operator=(const RWLock&) = delete;

};

}
