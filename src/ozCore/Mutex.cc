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

#include "Mutex.hh"

#include "System.hh"

#include <cstdlib>

#ifdef _WIN32
# include <windows.h>
#else
# include <pthread.h>
#endif

namespace oz
{

struct Mutex::Descriptor
{
#ifdef _WIN32
  HANDLE          mutex;
#else
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
};

Mutex::Mutex()
{
  descriptor = new(malloc(sizeof(Descriptor))) Descriptor;
  if (descriptor == nullptr) {
    OZ_ERROR("oz::Mutex: Descriptor initialisation failed");
  }

#ifdef _WIN32
  descriptor->mutex = CreateMutex(nullptr, false, nullptr);
#endif
}

Mutex::~Mutex()
{
#ifdef _WIN32
  CloseHandle(descriptor->mutex);
#else
  pthread_mutex_destroy(&descriptor->mutex);
#endif

  free(descriptor);
}

void Mutex::lock() const
{
#ifdef _WIN32
  WaitForSingleObject(descriptor->mutex, INFINITE);
#else
  pthread_mutex_lock(&descriptor->mutex);
#endif
}

bool Mutex::tryLock() const
{
#ifdef _WIN32
  return WaitForSingleObject(descriptor->mutex, 0) == WAIT_OBJECT_0;
#else
  return pthread_mutex_trylock(&descriptor->mutex) == 0;
#endif
}

void Mutex::unlock() const
{
#ifdef _WIN32
  ReleaseMutex(descriptor->mutex);
#else
  pthread_mutex_unlock(&descriptor->mutex);
#endif
}

}
