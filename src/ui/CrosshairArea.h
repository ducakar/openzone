/*
 *  CrosshairArea.h
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

  class CrosshairArea : public Area
  {
    private:

      int crossTexId;
      int grabTexId;
      int dim;

    protected:

      virtual void draw();

    public:

      CrosshairArea( int size );
      virtual ~CrosshairArea();

  };

}
}
}
