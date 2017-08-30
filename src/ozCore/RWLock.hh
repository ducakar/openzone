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
 * @sa `oz::SpinLock`, `oz::Mutex`
 */
class RWLock
{
public:

  /**
   * Accessor for reader (shared) lock.
   */
  class ReadLock
  {
    friend class RWLock;

  private:

    RWLock& parent_; ///< Parent shared mutex.

  private:

    /**
     * Create reader lock accessor for a given shared mutex.
     */
    explicit ReadLock(RWLock& parent)
      : parent_(parent)
    {}

    /**
     * No copying.
     */
    ReadLock(const ReadLock&) = delete;

    /**
     * No copying.
     */
    ReadLock& operator=(const ReadLock&) = delete;

  public:

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
  class WriteLock
  {
    friend class RWLock;

  private:

    RWLock& parent_; ///< Parent shared mutex.

  private:

    /**
     * Create writer lock accessor for a given shared mutex.
     */
    explicit WriteLock(RWLock& parent)
      : parent_(parent)
    {}

    /**
     * No copying.
     */
    WriteLock(const WriteLock&) = delete;

    /**
     * No copying.
     */
    WriteLock& operator=(const WriteLock&) = delete;

  public:

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

  ReadLock  read;  ///< Accessor for reader lock.
  WriteLock write; ///< Accessor for writer lock.

private:

  struct Descriptor;

  Descriptor* descriptor_ = nullptr; ///< Internal shared mutex descriptor.

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
