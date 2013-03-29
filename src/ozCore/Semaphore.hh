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
 * @file ozCore/Semaphore.hh
 *
 * `Semaphore` class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * %Semaphore.
 *
 * It is implemented as a wrapper for condition variable if a platform supports it since it should
 * yield a better performance.
 *
 * @sa `oz::SpinLock`, `oz::Mutex`, `oz::Thread`
 */
class Semaphore
{
  private:

    struct Descriptor;

    Descriptor* descriptor; ///< %Semaphore descriptor.

  public:

    /**
     * Create uninitialised instance.
     */
    explicit Semaphore() :
      descriptor( nullptr )
    {}

    /**
     * Destructor, destroys semaphore if initialised.
     */
    ~Semaphore()
    {
      if( descriptor != nullptr ) {
        destroy();
      }
    }

    /**
     * Move constructor.
     */
    Semaphore( Semaphore&& b ) :
      descriptor( b.descriptor )
    {
      b.descriptor = nullptr;
    }

    /**
     * Move operator.
     */
    Semaphore& operator = ( Semaphore&& b )
    {
      if( &b == this ) {
        return *this;
      }

      descriptor   = b.descriptor;
      b.descriptor = nullptr;

      return *this;
    }

    /**
     * True iff initialised.
     */
    bool isValid() const
    {
      return descriptor != nullptr;
    }

    /**
     * Get current counter value.
     */
    int counter() const;

    /**
     * Atomically increment counter and signal waiting threads.
     */
    void post() const;

    /**
     * Wait until counter becomes positive. Then atomically decrement it and resume.
     */
    void wait() const;

    /**
     * Atomically check if counter is positive and decrement it if it is.
     *
     * @return True iff counter was decremented.
     */
    bool tryWait() const;

    /**
     * Initialise semaphore.
     *
     * Initialising an already initialised semaphore is an error.
     *
     * @param counter initial counter value.
     */
    void init( int counter = 0 );

    /**
     * Destroy semaphore and release resources.
     *
     * Destroying uninitialised semaphore is a legal NOP.
     */
    void destroy();

};

}
