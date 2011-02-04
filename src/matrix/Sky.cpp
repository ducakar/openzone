/*
 *  Sky.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Sky.hpp"

#include "matrix/Timer.hpp"

namespace oz
{

  void Sky::update()
  {
    time = Math::mod( time + Timer::TICK_TIME, period );
  }

  void Sky::read( InputStream* istream )
  {
    time    = istream->readFloat();
    period  = istream->readFloat();
    heading = istream->readFloat();
  }

  void Sky::write( OutputStream* ostream )
  {
    ostream->writeFloat( time );
    ostream->writeFloat( period );
    ostream->writeFloat( heading );
  }

}
