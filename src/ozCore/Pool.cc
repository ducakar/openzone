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

#include "Alloc.hh"

namespace oz
{

union PoolAlloc::Slot
{
  Slot* nextSlot;
  char  storage[1];
};

struct PoolAlloc::Block
{
  Block* nextBlock;
  char   data[1] OZ_ALIGNED( OZ_ALIGNMENT );

  OZ_HIDDEN
  static Block* create( int slotSize, int nSlots, Block* nextBlock )
  {
    void*  chunk = new char[ OZ_ALIGNMENT + size_t( nSlots * slotSize ) ];
    Block* block = reinterpret_cast<Block*>( chunk );

    block->nextBlock = nextBlock;

    for( int i = 0; i < nSlots - 1; ++i ) {
      block->slot( i, slotSize )->nextSlot = block->slot( i + 1, slotSize );
    }
    block->slot( nSlots - 1, slotSize )->nextSlot = nullptr;

    return block;
  }

  OZ_HIDDEN
  void destroy()
  {
    delete[] reinterpret_cast<char*>( this );
  }

  OZ_HIDDEN
  OZ_ALWAYS_INLINE
  Slot* slot( int i, int slotSize )
  {
    return reinterpret_cast<Slot*>( &data[i * slotSize] );
  }
};

PoolAlloc::PoolAlloc( int slotSize_, int nSlots_ ) :
  firstBlock( nullptr ), freeSlot( nullptr ), slotSize( slotSize_ ), nSlots( nSlots_ ), count( 0 ),
  size( 0 )
{}

PoolAlloc::~PoolAlloc()
{
  free();
}

PoolAlloc::PoolAlloc( PoolAlloc&& p ) :
  firstBlock( p.firstBlock ), freeSlot( p.freeSlot ), slotSize( p.slotSize ), nSlots( p.nSlots ),
  count( p.count ), size( p.size )
{
  p.firstBlock = nullptr;
  p.freeSlot   = nullptr;
  p.slotSize   = 0;
  p.nSlots     = 0;
  p.count      = 0;
  p.size       = 0;
}

PoolAlloc& PoolAlloc::operator = ( PoolAlloc&& p )
{
  if( &p == this ) {
    return *this;
  }

  hard_assert( count == 0 );

  free();

  firstBlock   = p.firstBlock;
  freeSlot     = p.freeSlot;
  slotSize     = p.slotSize;
  nSlots       = p.nSlots;
  count        = p.count;
  size         = p.size;

  p.firstBlock = nullptr;
  p.freeSlot   = nullptr;
  p.slotSize   = 0;
  p.nSlots     = 0;
  p.count      = 0;
  p.size       = 0;

  return *this;
}

void* PoolAlloc::allocate()
{
  ++count;

  if( freeSlot == nullptr ) {
    firstBlock = Block::create( slotSize, nSlots, firstBlock );
    freeSlot   = firstBlock->slot( 1, slotSize );
    size      += slotSize;

    return firstBlock->slot( 0, slotSize )->storage;
  }
  else {
    Slot* slot = freeSlot;

    freeSlot = slot->nextSlot;
    return slot->storage;
  }
}

void PoolAlloc::deallocate( void* ptr )
{
  if( ptr == nullptr ) {
    return;
  }

  hard_assert( count != 0 );

  Slot* slot = static_cast<Slot*>( ptr );

#ifndef NDEBUG
  mSet( slot, 0xee, slotSize );
#endif

  slot->nextSlot = freeSlot;
  freeSlot = slot;
  --count;
}

void PoolAlloc::free()
{
  if( firstBlock == nullptr ) {
    return;
  }

  soft_assert( count == 0 );

  if( count == 0 ) {
    Block* block = firstBlock;

    while( block != nullptr ) {
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
