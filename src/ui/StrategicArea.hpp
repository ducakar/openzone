/*
 *  StrategicArea.hpp
 *
 *  Real-time strategy interface layer for HUD.
 *  Provides unit selection and commanding.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "ui/Area.hpp"

#include "matrix/common.hpp"

namespace oz
{
  class Object;
}

namespace oz
{
namespace client
{
namespace ui
{

  class StrategicArea : public Area
  {
    private:

      static const float TAG_CLIP_DIST;

      float pixelStep;
      float stepPixel;

    public:

      const Object* hovered;
      Vector<int>   tagged;

    private:

      Pair<int> project( const Point3& p ) const;
      Span projectBounds( const AABB& bb ) const;

      void printName( int baseX, int baseY, const char* s, ... );

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
