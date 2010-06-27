/*
 *  StrategicArea.hh
 *
 *  Real-time strategy interface layer for HUD.
 *  Provides unit selection and commanding.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

#include "matrix/Object.hh"
#include "ui/Area.hh"

namespace oz
{
namespace client
{
namespace ui
{

  class StrategicArea : public Area
  {
    private:

      float         pixelStep;
      float         stepPixel;
      bool          leftClick;

    public:

      const Object* hovered;
      Vector<int>   tagged;

    private:

      Pair<int> project( const Vec3& p ) const;
      void projectBounds( Span& span, const AABB& bb ) const;

      void printName( int baseX, int baseY, const char* s, ... );
      static void fillRect( float x, float y, float width, float height );
      static void drawRect( float x, float y, float width, float height );

      void drawHoveredRect( const Span& span );
      void drawTaggedRect( const Object* obj, const Span& span );

    protected:

      virtual void onUpdate();
      virtual bool onMouseEvent();
      virtual void onDraw();

    public:

      explicit StrategicArea();
      virtual ~StrategicArea();

  };

}
}
}
