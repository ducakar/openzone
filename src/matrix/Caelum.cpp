/*
 *  Caelum.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Caelum.hpp"

#include "matrix/Timer.hpp"

namespace oz
{

  Caelum::Caelum() : id( -1 ), heading( 0.0f ), period( 86400.0f ), time( 0.0f )
  {}

  void Caelum::update()
  {
    time = Math::mod( time + Timer::TICK_TIME, period );
  }

  void Caelum::read( InputStream* istream )
  {
    id      = istream->readInt();
    heading = istream->readFloat();
    period  = istream->readFloat();
    time    = istream->readFloat();
  }

  void Caelum::write( OutputStream* ostream ) const
  {
    ostream->writeInt( id );
    ostream->writeFloat( heading );
    ostream->writeFloat( period );
    ostream->writeFloat( time );
  }

}
