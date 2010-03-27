/*
 *  Sky.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "stable.h"

#include "matrix/Sky.h"

namespace oz
{

  void Sky::set( float heading_, float period_, float time_ )
  {
    heading = heading_;
    period  = period_;
    time    = time_;
  }

  void Sky::update()
  {
    time = Math::mod( time + Timer::TICK_TIME, period );
  }

  void Sky::read( InputStream* istream )
  {
    heading = istream->readFloat();
    period  = istream->readFloat();
    time    = istream->readFloat();
  }

  void Sky::write( OutputStream* ostream )
  {
    ostream->writeFloat( heading );
    ostream->writeFloat( period );
    ostream->writeFloat( time );
  }

}
