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

  Pool<Memo, 1024> Memo::pool;

  Device* Memo::create( int, InputStream* istream )
  {
    return new Memo( istream->readString() );
  }

  Memo::Memo( const char* text_ ) : Device( MEMO_BIT ), text( text_ )
  {}

  const char* Memo::type() const
  {
    return "Memo";
  }

  const char* Memo::getMemo() const
  {
    return text;
  }

  void Memo::write( OutputStream* ostream ) const
  {
    ostream->writeString( text );
  }

}
}
