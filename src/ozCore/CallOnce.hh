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
 * @file ozCore/CallOnce.hh
 *
 * `CallOnce` class.
 */

#pragma once

#ifndef EMSCRIPTEN

#include "common.hh"

namespace oz
{

/**
 * Call a method only once, even if called from several threads.
 *
 * @sa `oz::SpinLock`, `oz::Mutex`, `oz::Semaphore`, `oz::Thread`
 */
class CallOnce
{
  public:

    /// %Method type.
    typedef void Method();

  private:

    volatile int  onceLock;        ///< Spin lock.
    volatile bool wasMethodCalled; ///< Whether the method was already called.

  public:

    /**
     * Create a new instance.
     */
    explicit CallOnce() :
      onceLock( 0 ), wasMethodCalled( false )
    {}

    /**
     * Copying or moving is not possible for the embedded spin lock.
     */
    CallOnce( const CallOnce& ) = delete;

    /**
     * Copying or moving is not possible for the embedded spin lock.
     */
    CallOnce& operator = ( const CallOnce& ) = delete;

    /**
     * True this object has already been used to call a method.
     */
    OZ_ALWAYS_INLINE
    bool wasCalled() const
    {
      return wasMethodCalled;
    }

    /**
     * Call method if this is the first call on this object.
     *
     * The `call()` method only executes the given method first time it is called on this object.
     * Internal locking mechanism prevents race conditions when this is done from two threads
     * simultaneously.
     *
     * If `method` is null, this object is just set as used, as if it would have already called a
     * method.
     */
    void call( Method* method );

};

}

#endif // EMSCRIPTEN
