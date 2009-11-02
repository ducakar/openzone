/*
 *  Sky.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Timer.h"
#include "io.h"

namespace oz
{

  struct Sky
  {
    float heading;
    float period;
    float time;

    // heading of sun in degrees and day time in seconds
    void set( float heading, float period, float time );
    void update();

    void read( InputStream *istream );
    void write( OutputStream *ostream );
  };

}
