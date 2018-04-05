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
  static Block* create(int slotSize, int blockSlots, Block* nextBlock)
  {
    char*  chunk = new char[OZ_ALIGNMENT + blockSlots * slotSize];
    Block* block = new(chunk) Block;

    block->nextBlock = nextBlock;

    for (int i = 0; i < blockSlots - 1; ++i) {
      block->slot(i, slotSize)->nextSlot = block->slot(i + 1, slotSize);
    }
    block->slot(blockSlots - 1, slotSize)->nextSlot = nullptr;

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

PoolAlloc::PoolAlloc(int slotSize, int blockSlots)
  : slotSize_(slotSize), blockSlots_(blockSlots)
{}

PoolAlloc::~PoolAlloc()
{
  free();
}

PoolAlloc::PoolAlloc(PoolAlloc&& other) noexcept
  : firstBlock_(other.firstBlock_), freeSlot_(other.freeSlot_), slotSize_(other.slotSize_),
    blockSlots_(other.blockSlots_), size_(other.size_), capacity_(other.capacity_)
{
  OZ_MOVE_CTOR_BODY(PoolAlloc);
}

PoolAlloc& PoolAlloc::operator=(PoolAlloc&& other) noexcept
{
  OZ_MOVE_OP_BODY(PoolAlloc);
}

void* PoolAlloc::allocate()
{
  ++size_;

  if (freeSlot_ == nullptr) {
    firstBlock_ = Block::create(slotSize_, blockSlots_, firstBlock_);
    freeSlot_   = firstBlock_->slot(1, slotSize_);
    capacity_  += blockSlots_;

    return firstBlock_->slot(0, slotSize_)->storage;
  }

  Slot* slot = freeSlot_;
  freeSlot_ = slot->nextSlot;
  return slot->storage;
}

void PoolAlloc::deallocate(void* ptr)
{
  if (ptr == nullptr) {
    return;
  }

  OZ_ASSERT(size_ != 0);

  Slot* slot = static_cast<Slot*>(ptr);

#ifndef NDEBUG
  memset(slot, 0xee, slotSize_);
#endif

  slot->nextSlot = freeSlot_;
  freeSlot_ = slot;
  --size_;
}

void PoolAlloc::free()
{
  OZ_ASSERT(size_ == 0);

  if (size_ == 0) {
    for (Block* block = firstBlock_; block != nullptr;) {
      Block* next = block->nextBlock;

      block->destroy();
      block = next;
    }
  }

  firstBlock_ = nullptr;
  freeSlot_   = nullptr;
  size_       = 0;
  capacity_   = 0;
}

}
