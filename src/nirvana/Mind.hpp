/*
 *  Mind.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "nirvana/common.hpp"

namespace oz
{
namespace nirvana
{

class Mind
{
  public:

    static const int FORCE_UPDATE_BIT = 0x00000001;

    static Pool<Mind, 1024> pool;

    Mind* prev[1];
    Mind* next[1];

    int flags;
    int bot;

    explicit Mind( int bot );
    explicit Mind( int bot, InputStream* istream );
    ~Mind();

    void update();

    void write( OutputStream* ostream ) const;

  OZ_STATIC_POOL_ALLOC( pool )

};

}
}
