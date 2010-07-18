/*
 *  Mind.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "nirvana/common.hpp"
#include "nirvana/Task.hpp"

namespace oz
{
namespace nirvana
{

  class RandomMind;
  class LuaMind;

  class Mind
  {
    friend class DList<Mind>;
    friend class Pool<Mind>;

    public:

      static const int FORCE_UPDATE_BIT = 0x00000001;

      typedef Mind* ( *CreateFunc )( int bot );
      typedef Mind* ( *ReadFunc )( InputStream* istream );

      static Pool<Mind> pool;

      Mind* prev[1];
      Mind* next[1];

      int   flags;
      int   botIndex;

      static Mind* create( int botIndex );
      static Mind* read( InputStream* istream );

      Mind( int botIndex_ ) : flags( 0 ), botIndex( botIndex_ ) {}
      virtual ~Mind();

      virtual const char* type() const;
      virtual void update();

      virtual void write( OutputStream* ostream ) const;

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
