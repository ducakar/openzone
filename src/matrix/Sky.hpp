/*
 *  Sky.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/common.hpp"
#include "matrix/Timer.hpp"

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
