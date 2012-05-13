/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file oz/Semaphore.hh
 *
 * Semaphore class.
 */

#pragma once

#include "common.hh"

namespace oz
{

// Internal structure for semaphore description.
struct SemaphoreDesc;

/**
 * Semaphore.
 *
 * It is implemented as a wrapper for condition variable as it should yield better performance.
 *
 * @ingroup oz
 */
class Semaphore
{
  private:

    /// %Semaphore descriptor.
    SemaphoreDesc* descriptor;

  public:

    /**
     * Create uninitialised instance.
     */
    Semaphore() :
      descriptor( null )
    {}

    /**
     * Destructor.
     */
    ~Semaphore()
    {
      soft_assert( descriptor == null );
    }

    /**
     * No copying.
     */
    Semaphore( const Semaphore& ) = delete;

    /**
     * Move constructor, transfers ownership.
     */
    Semaphore( Semaphore&& b ) :
      descriptor( b.descriptor )
    {
      b.descriptor = null;
    }

    /**
     * No copying.
     */
    Semaphore& operator = ( const Semaphore& ) = delete;

    /**
     * Move operator, transfers ownership.
     */
    Semaphore& operator = ( Semaphore&& b )
    {
      descriptor   = b.descriptor;
      b.descriptor = null;
      return *this;
    }

    /**
     * True iff initialised.
     */
    bool isValid() const
    {
      return descriptor != null;
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
     * If counter is positive decrement it and return true, otherwise resume and return false.
     */
    bool tryWait() const;

    /**
     * Initialise semaphore.
     *
     * @param counter initial counter value.
     */
    void init( int counter = 0 );

    /**
     * Destroy semaphore and release resources.
     */
    void destroy();

};


}
