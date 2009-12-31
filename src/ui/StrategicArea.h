/*
 *  StrategicArea.h
 *
 *  Real-time strategy interface layer for HUD.
 *  Provides unit selection and commanding.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "matrix/Object.h"
#include "Area.h"

namespace oz
{
namespace client
{
namespace ui
{

  struct StrategicArea : Area
  {
    private:

      float       pixelStep;
      float       stepPixel;

      int         hover;
      Vector<int> tagged;

      Pair<int> project( const Vec3& p ) const;
      void projectBounds( oz::Area& area, const Object* obj ) const;

      void end();

    protected:

      virtual void onUpdate();
      virtual void onDraw();

    public:

      explicit StrategicArea();
      virtual ~StrategicArea();

      void begin();

  };

}
}
}
