/*
 *  NamePool.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "matrix/common.hpp"

namespace oz
{
namespace matrix
{

class NamePool
{
  private:

    static const int LINE_LENGTH = 64;

    static char    buffer[LINE_LENGTH];

    Vector<String> names;
    Vector<int>    listPositions;

  public:

    const String& genName( int list ) const;

    void init();
    void free();

};

extern NamePool namePool;

}
}
