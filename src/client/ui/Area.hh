/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file client/ui/Area.hh
 */

#pragma once

#include <client/common.hh>

namespace oz
{
namespace client
{
namespace ui
{

class Area
{
  friend class DChain<Area>;
  friend class UI;

  public:

    static const int CENTRE          = 0x10000000; ///< Special value for x and y.

    static const int UPDATE_BIT      = 0x01;       ///< If `onUpdate()` should be called.
    static const int PINNED_BIT      = 0x02;       ///< Show in both freelook and interface mode.
    static const int GRAB_BIT        = 0x04;       ///< A child with GRAB_BIT get exclusive focus
                                                   ///< for events
    static const int ENABLED_BIT     = 0x10;       ///< Visibility and `onUpdate()` enabled.
    static const int VISIBLE_BIT     = 0x20;       ///< Visible.

    // text alignment
    static const int ALIGN_LEFT      = 0x00;
    static const int ALIGN_RIGHT     = 0x01;
    static const int ALIGN_HCENTRE   = 0x02;
    static const int ALIGN_BOTTOM    = 0x00;
    static const int ALIGN_TOP       = 0x04;
    static const int ALIGN_VCENTRE   = 0x08;
    static const int ALIGN_NONE      = ALIGN_LEFT | ALIGN_BOTTOM;
    static const int ALIGN_CENTRE    = ALIGN_HCENTRE | ALIGN_VCENTRE;

  protected:

    Area* prev[1];
    Area* next[1];

    int   flags;

  public:

    Area*        parent;
    DChain<Area> children;

    // absolute x and y, not relative to parent
    int x;
    int y;
    int width;
    int height;

  protected:

    int defaultX;
    int defaultY;

    void updateChildren();
    // return true if event has been caught
    bool passMouseEvents();
    bool passKeyEvents();
    void drawChildren();

    virtual void onVisibilityChange( bool doShow );
    virtual void onReposition();
    virtual void onUpdate();
    // return true if event has been caught
    virtual bool onMouseEvent();
    virtual bool onKeyEvent();
    virtual void onDraw();

  public:

    explicit Area( int width, int height );
    virtual ~Area();

    OZ_ALWAYS_INLINE
    bool isEnabled() const
    {
      return flags & ENABLED_BIT;
    }

    void enable( bool doEnable );

    OZ_ALWAYS_INLINE
    bool isVisible() const
    {
      return ( flags & ( ENABLED_BIT | VISIBLE_BIT ) ) == ( ENABLED_BIT | VISIBLE_BIT );
    }

    void show( bool doShow );

    /**
     * Fix position if parent has been resized.
     */
    void reposition();

    /**
     * Move for `moveX` to the right and for `moveY` up.
     */
    void move( int moveX, int moveY );

    /**
     * Calculate global (x, y) for given relative rectangle coordinates/dimension.
     */
    Pair<int> align( int localX, int localY, int width, int height ) const;

    void add( Area* area, int localX, int localY );
    void remove( Area* area );
    void raise();
    void sink();

};

}
}
}
