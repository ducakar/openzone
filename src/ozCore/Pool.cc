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
 * @file ozCore/Pool.cc
 */

#include "Pool.hh"

#include "System.hh"

#include <cstring>

namespace oz
{

union alignas(OZ_ALIGNMENT) PoolAlloc::Slot
{
  Slot* nextSlot;
  char  storage[1];
};

struct PoolAlloc::Block
{
  Block*             nextBlock;
  alignas(Slot) char data[1];

  OZ_INTERNAL
  static Block* create(int slotSize, int nSlots, Block* nextBlock)
  {
    char*  chunk = new char[OZ_ALIGNMENT + nSlots * slotSize];
    Block* block = new(chunk) Block;

    block->nextBlock = nextBlock;

    for (int i = 0; i < nSlots - 1; ++i) {
      block->slot(i, slotSize)->nextSlot = block->slot(i + 1, slotSize);
    }
    block->slot(nSlots - 1, slotSize)->nextSlot = nullptr;

    return block;
  }

  OZ_INTERNAL
  void destroy()
  {
    delete[] reinterpret_cast<char*>(this);
  }

  OZ_INTERNAL
  OZ_ALWAYS_INLINE
  Slot* slot(int i, int slotSize)
  {
    return reinterpret_cast<Slot*>(&data[i * slotSize]);
  }
};

PoolAlloc::PoolAlloc(int slotSize, int blockSlots) :
  objectSize(slotSize), nSlots(blockSlots)
{}

PoolAlloc::~PoolAlloc()
{
  free();
}

PoolAlloc::PoolAlloc(PoolAlloc&& p) :
  firstBlock(p.firstBlock), freeSlot(p.freeSlot), objectSize(p.objectSize), nSlots(p.nSlots),
  count(p.count), size(p.size)
{
  p.firstBlock = nullptr;
  p.freeSlot   = nullptr;
  p.objectSize = 0;
  p.nSlots     = 0;
  p.count      = 0;
  p.size       = 0;
}

PoolAlloc& PoolAlloc::operator = (PoolAlloc&& p)
{
  if (&p != this) {
    free();

    firstBlock = p.firstBlock;
    freeSlot   = p.freeSlot;
    objectSize = p.objectSize;
    nSlots     = p.nSlots;
    count      = p.count;
    size       = p.size;

    p.firstBlock = nullptr;
    p.freeSlot   = nullptr;
    p.objectSize = 0;
    p.nSlots     = 0;
    p.count      = 0;
    p.size       = 0;
  }
  return *this;
}

void* PoolAlloc::allocate()
{
  ++count;

  if (freeSlot == nullptr) {
    firstBlock = Block::create(objectSize, nSlots, firstBlock);
    freeSlot   = firstBlock->slot(1, objectSize);
    size      += nSlots;

    return firstBlock->slot(0, objectSize)->storage;
  }
  else {
    Slot* slot = freeSlot;

    freeSlot = slot->nextSlot;
    return slot->storage;
  }
}

void PoolAlloc::deallocate(void* ptr)
{
  if (ptr == nullptr) {
    return;
  }

  OZ_ASSERT(count != 0);

  Slot* slot = static_cast<Slot*>(ptr);

#ifndef NDEBUG
  memset(slot, 0xee, objectSize);
#endif

  slot->nextSlot = freeSlot;
  freeSlot = slot;
  --count;
}

void PoolAlloc::free()
{
  OZ_ASSERT(count == 0);

  if (count == 0) {
    for (Block* block = firstBlock; block != nullptr;) {
      Block* next = block->nextBlock;

      block->destroy();
      block = next;
    }
  }

  firstBlock = nullptr;
  freeSlot   = nullptr;
  count      = 0;
  size       = 0;
}

}
