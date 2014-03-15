/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file ozCore/MainCall.hh
 */

#if 0

#include "MainCall.hh"

#include "SpinLock.hh"

namespace oz
{

static const int MAX_CLOSURES = 256;

typedef void Callback( void* data );

struct Closure
{
  Callback* callback;
  void*     data;
};

static Semaphore     mainSemaphore;
static SpinLock      closuresLock;
static Closure       closures[MAX_CLOSURES];
static int           firstClosure = 0;
static int           pastClosure  = 0;
static volatile int  nClosures    = 0;
static volatile bool isAlive      = false;

MainCall::LocalSemaphore::LocalSemaphore()
{
  sem.init();
}

MainCall::LocalSemaphore::~LocalSemaphore()
{
  sem.destroy();
}

thread_local MainCall::LocalSemaphore MainCall::localSemaphore;

bool MainCall::schedule( Callback* callback, void* data )
{
  closuresLock.lock();

  if( nClosures == MAX_CLOSURES ) {
    closuresLock.unlock();
    return false;
  }

  Closure& closure = closures[pastClosure];
  closure.callback = callback;
  closure.data     = data;

  pastClosure = ( pastClosure + 1 ) % MAX_CLOSURES;
  ++nClosures;

  closuresLock.unlock();

  mainSemaphore.post();
  return true;
}

void MainCall::loop()
{
  isAlive = true;

  while( isAlive )
  {
    mainSemaphore.wait();

    if( nClosures != 0 ) {
      Closure& closure = closures[firstClosure];
      closure.callback( closure.data );

      firstClosure = ( firstClosure + 1 ) % MAX_CLOSURES;

      closuresLock.lock();
      --nClosures;
      closuresLock.unlock();
    }
  }
}

void MainCall::wait()
{
  if( nClosures != 0 ) {
    MainCall() << []()
    {};
  }
}

void MainCall::terminate()
{
  MainCall() << []()
  {
    isAlive = false;
  };
}

void MainCall::init()
{
  mainSemaphore.init();
}

void MainCall::destroy()
{
  mainSemaphore.destroy();
}

}

#endif
