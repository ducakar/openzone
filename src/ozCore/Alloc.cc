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
 * @file ozCore/Alloc.cc
 */

#include "Alloc.hh"

#include "arrays.hh"
#include "System.hh"
#include "SpinLock.hh"

#include <cstdlib>
#include <malloc.h>

namespace oz
{

#ifdef OZ_ALLOCATOR

enum AllocMode
{
  OBJECT,
  ARRAY
};

static Chain<Alloc::ChunkInfo> chunkInfos[2];
static SpinLock                allocInfoLock;

static void* allocate(AllocMode mode, size_t size)
{
#ifdef OZ_SIMD_MATH
  size = Alloc::alignUp(size);
#endif
  size += Alloc::alignUp(sizeof(Alloc::ChunkInfo));

#if defined(OZ_SIMD_MATH) && defined(_WIN32)
  void* ptr = _aligned_malloc(size, Alloc::ALIGNMENT);
#elif defined(OZ_SIMD_MATH)
  void* ptr = alligned_alloc(Alloc::ALIGNMENT, size);
#else
  void* ptr = malloc(size);
#endif

  if (ptr == nullptr) {
    OZ_ERROR("oz::Alloc: Out of memory");
  }

  Alloc::ChunkInfo* ci = static_cast<Alloc::ChunkInfo*>(ptr);
  ci->size       = size;
  ci->stackTrace = StackTrace::current(2);

  allocInfoLock.lock();

  chunkInfos[mode].add(ci);

  ++Alloc::count;
  Alloc::amount += size;

  ++Alloc::sumCount;
  Alloc::sumAmount += size;

  Alloc::maxCount  = max<int>(Alloc::count, Alloc::maxCount);
  Alloc::maxAmount = max<size_t>(Alloc::amount, Alloc::maxAmount);

  allocInfoLock.unlock();

  return static_cast<char*>(ptr) + Alloc::alignUp(sizeof(Alloc::ChunkInfo));
}

static void deallocate(AllocMode mode, void* ptr)
{
  ptr = static_cast<char*>(ptr) - Alloc::alignUp(sizeof(Alloc::ChunkInfo));

  allocInfoLock.lock();

  Alloc::ChunkInfo* ci   = static_cast<Alloc::ChunkInfo*>(ptr);
  Alloc::ChunkInfo* prev = chunkInfos[mode].before(ci);

  if (prev != nullptr || chunkInfos[mode].first() == ci) {
    chunkInfos[mode].erase(ci, prev);
  }
  else {
    // Check if allocated as a different kind (object/array)
    if (chunkInfos[!mode].has(ci)) {
      OZ_ERROR("oz::Alloc: new[] -> delete mismatch for %s block at %p of size %lu",
               mode == OBJECT ? "object" : "array", ptr, ulong(ci->size));
    }
    else {
      OZ_ERROR("oz::Alloc: Freeing unregistered %s block at %p of size %lu",
               mode == OBJECT ? "object" : "array", ptr, ulong(ci->size));
    }
  }

  --Alloc::count;
  Alloc::amount -= ci->size;

  allocInfoLock.unlock();

  mSet(ptr, 0xee, int(ci->size));

#if defined(OZ_SIMD_MATH) && defined(_WIN32)
  _aligned_free(ptr));
#else
  free(ptr);
#endif
}

#endif // OZ_ALLOCATOR

int    Alloc::count     = 0;
size_t Alloc::amount    = 0;
int    Alloc::sumCount  = 0;
size_t Alloc::sumAmount = 0;
int    Alloc::maxCount  = 0;
size_t Alloc::maxAmount = 0;

Alloc::CIterator Alloc::objectCIter()
{
#ifdef OZ_ALLOCATOR
  return chunkInfos[OBJECT].citer();
#else
  return CIterator();
#endif
}

Alloc::CIterator Alloc::arrayCIter()
{
#ifdef OZ_ALLOCATOR
  return chunkInfos[ARRAY].citer();
#else
  return CIterator();
#endif
}

}

#ifdef OZ_ALLOCATOR

OZ_WEAK
void* operator new (size_t size)
{
  return oz::allocate(oz::OBJECT, size);
}

OZ_WEAK
void* operator new[] (size_t size)
{
  return oz::allocate(oz::ARRAY, size);
}

OZ_WEAK
void operator delete (void* ptr) noexcept
{
  if (ptr == nullptr) {
    return;
  }
  oz::deallocate(oz::OBJECT, ptr);
}

OZ_WEAK
void operator delete[] (void* ptr) noexcept
{
  if (ptr == nullptr) {
    return;
  }
  oz::deallocate(oz::ARRAY, ptr);
}

OZ_WEAK
void* operator new (size_t size, const std::nothrow_t&) noexcept
{
  return oz::allocate(oz::OBJECT, size);
}

OZ_WEAK
void* operator new[] (size_t size, const std::nothrow_t&) noexcept
{
  return oz::allocate(oz::ARRAY, size);
}

OZ_WEAK
void operator delete (void* ptr, const std::nothrow_t&) noexcept
{
  if (ptr == nullptr) {
    return;
  }
  oz::deallocate(oz::OBJECT, ptr);
}

OZ_WEAK
void operator delete[] (void* ptr, const std::nothrow_t&) noexcept
{
  if (ptr == nullptr) {
    return;
  }
  oz::deallocate(oz::ARRAY, ptr);
}

#endif // OZ_ALLOCATOR
