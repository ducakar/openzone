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

      float         pixelStep;
      float         stepPixel;

      const Object* hovered;
      Vector<int>   tagged;

      Pair<int> project( const Vec3& p ) const;
      void projectBounds( Span& span, const Object* obj ) const;

      static void fillRect( float x, float y, float width, float height );
      static void drawRect( float x, float y, float width, float height );

      void drawHoveredRect( const Span& span );
      void drawTaggedRect( const Object* obj, const Span& span );

    protected:

      virtual void onUpdate();
      virtual void onDraw();

    public:

      explicit StrategicArea();
      virtual ~StrategicArea();

  };

}
}
}
