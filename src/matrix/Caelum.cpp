/*
 *  Caelum.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Caelum.hpp"

#include "matrix/Timer.hpp"

namespace oz
{

  void Caelum::update()
  {
    time = Math::mod( time + Timer::TICK_TIME, period );
  }

  void Caelum::read( InputStream* istream )
  {
    heading = istream->readFloat();
    period  = istream->readFloat();
    time    = istream->readFloat();
  }

  void Caelum::write( OutputStream* ostream )
  {
    ostream->writeFloat( heading );
    ostream->writeFloat( period );
    ostream->writeFloat( time );
  }

}
