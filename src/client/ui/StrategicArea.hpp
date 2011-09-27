/*
 *  StrategicArea.hpp
 *
 *  Real-time strategy interface layer for HUD.
 *  Provides unit selection and commanding.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/ui/Area.hpp"

#include "matrix/Object.hpp"

#include "client/common.hpp"

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
      static const float TAG_CLAMP_LIMIT;
      static const float TAG_MIN_PIXEL_SIZE;
      static const float TAG_MAX_COEFF_SIZE;

      uint  titleTexId;
      float pixelStep;
      float stepPixel;

    public:

      Vector<int> tagged;
      const Object* hovered;

    private:

      bool projectBounds( Span* span, const AABB& bb ) const;
      void printName( int baseX, int baseY, const char* s, ... );

      void drawHoveredRect( const Span& span );
      void drawTaggedRect( const Object* obj, const Span& span );

    protected:

      virtual void onUpdate();
      virtual bool onMouseEvent();
      virtual void onDraw();

    public:

      StrategicArea();
      virtual ~StrategicArea();

  };

}
}
}
