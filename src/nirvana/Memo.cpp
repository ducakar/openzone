/*
 *  Device.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "nirvana/Memo.hpp"

namespace oz
{
namespace nirvana
{

  Memo::Memo( const char* text_ ) : Device( MEMO_BIT ), text( text_ )
  {}

  const char* Memo::getMemo() const
  {
    return text;
  }

}
}
