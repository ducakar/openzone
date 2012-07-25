/*
 *  CrosshairArea.h
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

  class CrosshairArea : public Area
  {
    private:

      int crossTexId;
      int useTexId;
      int mountTexId;
      int grabTexId;
      int dim;

    protected:

      virtual void draw();

    public:

      bool showUse;
      bool showMount;

      CrosshairArea( int size );
      virtual ~CrosshairArea();

  };

}
}
}
