/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file ozCore/SpinLock.hh
 *
 * `SpinLock` class.
 */

#pragma once

#ifndef EMSCRIPTEN

#include "common.hh"

namespace oz
{

/**
 * Spin lock.
 *
 * @sa `oz::Mutex`, `oz::Semaphore`, `oz::CallOnce`, `oz::Thread`
 */
class SpinLock
{
  private:

    volatile int value; ///< 0 - unlocked, 1 - locked.

  public:

    /**
     * Create new instance.
     */
    OZ_ALWAYS_INLINE
    explicit SpinLock() :
      value( 0 )
    {}

    /**
     * Destructor.
     */
    OZ_ALWAYS_INLINE
    ~SpinLock()
    {
      hard_assert( value == 0 );
    }

    /**
     * Copying or moving is not possible.
     */
    SpinLock( const SpinLock& ) = delete;

    /**
     * Copying or moving is not possible.
     */
    SpinLock& operator = ( const SpinLock& ) = delete;

    /**
     * True iff locked.
     */
    OZ_ALWAYS_INLINE
    bool isLocked() const
    {
      return value != 0;
    }

    /**
     * Loop performing a lock operation until it switches from an unlocked to a locked state.
     */
    OZ_ALWAYS_INLINE
    void lock()
    {
      while( __sync_lock_test_and_set( &value, 1 ) != 0 ) {
        while( value != 0 );
      }
    }

    /**
     * Atomically lock and check if it has already been locked.
     *
     * @return True iff it was unlocked.
     */
    OZ_ALWAYS_INLINE
    bool tryLock()
    {
      return __sync_lock_test_and_set( &value, 1 ) == 0;
    }

    /**
     * Unlock.
     */
    OZ_ALWAYS_INLINE
    void unlock()
    {
      __sync_lock_release( &value );
    }

};

}

#endif // EMSCRIPTEN
