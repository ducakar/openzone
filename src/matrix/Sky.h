/*
 *  Sky.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "matrix/common.h"
#include "matrix/Timer.h"

namespace oz
{

  class Sky
  {
    public:

      float heading;
      float period;
      float time;

      // heading of sun in degrees and day time in seconds
      void set( float heading, float period, float time );
      void update();

      void read( InputStream* istream );
      void write( OutputStream* ostream );

  };

}
