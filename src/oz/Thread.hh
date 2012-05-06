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
 * @file oz/Thread.hh
 *
 * Classes for thread manipulation and synchronisation.
 */

#pragma once

#include "common.hh"

namespace oz
{

// Internal structure for mutex description.
struct MutexDesc;

// Internal structure for semaphore description.
struct SemaphoreDesc;

// Internal structure for thread description.
struct ThreadDesc;

/**
 * %Mutex.
 *
 * @ingroup oz
 */
class Mutex
{
  private:

    /// %Mutex descriptor.
    MutexDesc* descriptor;

  public:

    /**
     * Create uninitialised instance.
     */
    Mutex() :
      descriptor( null )
    {}

    /**
     * Destructor.
     */
    ~Mutex()
    {
      soft_assert( descriptor == null );
    }

    /**
     * No copying.
     */
    Mutex( const Mutex& ) = delete;

    /**
     * Move constructor, transfers ownership.
     */
    Mutex( Mutex&& m ) :
      descriptor( m.descriptor )
    {
      m.descriptor = null;
    }

    /**
     * No copying.
     */
    Mutex& operator = ( const Mutex& ) = delete;

    /**
     * Move operator, transfers ownership.
     */
    Mutex& operator = ( Mutex&& m )
    {
      descriptor   = m.descriptor;
      m.descriptor = null;
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
     * Initialise mutex.
     */
    void init();

    /**
     * Destroy mutex and release resources.
     */
    void destroy();

    /**
     * Lock mutex.
     */
    void lock() const;

    /**
     * Try to lock mutex.
     */
    bool tryLock() const;

    /**
     * Unlock mutex.
     */
    void unlock() const;

};

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
     * Initialise semaphore.
     */
    void init( int counterValue = 0 );

    /**
     * Destroy semaphore and release resources.
     */
    void destroy();

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

};

/**
 * %Thread.
 */
class Thread
{
  public:

    /// %Thread's main function.
    typedef void Main();

  private:

    /// %Thread descriptor.
    ThreadDesc* descriptor;

  public:

    /**
     * Create instance.
     */
    Thread() :
      descriptor( null )
    {}

    /**
     * No copying.
     */
    Thread( const Thread& ) = delete;

    /**
     * Move constructor, transfers ownership.
     */
    Thread( Thread&& t ) :
      descriptor( t.descriptor )
    {
      t.descriptor = null;
    }

    /**
     * No copying.
     */
    Thread& operator = ( const Thread& ) = delete;

    /**
     * Move operator, transfers ownership.
     */
    Thread& operator = ( Thread&& t )
    {
      descriptor   = t.descriptor;
      t.descriptor = null;
      return *this;
    }

    /**
     * True iff thread has been started but not yet joined.
     */
    bool isValid() const
    {
      return descriptor != null;
    }

    /**
     * Run thread.
     *
     * @param main pointer to thread's main function.
     */
    void start( Main* main );

    /**
     * Wait for thread to finish execution.
     */
    void join();

};

}
