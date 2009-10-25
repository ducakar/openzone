/*
 *  Mind.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Mind.h"

namespace oz
{

  Mind *Mind::create( int botIndex )
  {
    Mind *mind = new Mind( botIndex );
    return mind;
  }

  Mind *Mind::read( InputStream *istream )
  {
    Mind *mind = new Mind( istream->readInt() );
    return mind;
  }

  Mind::~Mind()
  {}

  const char *Mind::type() const
  {
    return "";
  }

  void Mind::update()
  {}

  void Mind::write( OutputStream *ostream ) const
  {
    ostream->writeInt( botIndex );
  }

}
