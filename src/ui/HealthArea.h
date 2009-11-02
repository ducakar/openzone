/*
 *  HealthArea.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Area.h"

namespace oz
{
namespace client
{
namespace ui
{

  class HealthArea : public Area
  {
    protected:

      virtual void draw();

    public:

      HealthArea() : Area( -260, 10, 250, 50 ) {}

  };

}
}
}
