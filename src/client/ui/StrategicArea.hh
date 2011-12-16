/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/ui/StrategicArea.hh
 *
 * Real-time strategy interface layer for HUD.
 * Provides unit selection and commanding.
 */

#pragma once

#include "matrix/Object.hh"

#include "client/ui/Area.hh"

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

    Vector<int> taggedStrs;
    Vector<int> taggedObjs;

    int hoverStr;
    int hoverObj;

  private:

    bool projectBounds( Span* span, const AABB& bb ) const;

    OZ_PRINTF_FORMAT( 4, 5 )
    void printName( int baseX, int baseY, const char* s, ... );

    void drawHoveredRect( const Span& span, const Struct* str, const Object* obj );
    void drawTaggedRect( const Span& span, const Struct* str, const Object* obj, bool isHovered );

  protected:

    virtual void onUpdate();
    virtual void onVisibilityChange();
    virtual bool onMouseEvent();
    virtual void onDraw();

  public:

    StrategicArea();
    virtual ~StrategicArea();

};

}
}
}
