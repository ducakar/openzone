/*
 *  HealthArea.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
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
    public:

      HealthArea() : Area( -260, 10, 250, 50 ) {}

      virtual void draw();

  };

}
}
}
