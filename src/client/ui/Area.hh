/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

namespace oz::client::ui
{

class Area : public DChainNode<Area>
{
  friend class UI;

public:

  static constexpr int CENTRE          = 0x10000000; ///< Special bit for x and y value.

  static constexpr int UPDATE_BIT      = 0x01;       ///< If `onUpdate()` should be called.
  static constexpr int PINNED_BIT      = 0x02;       ///< Show in both freelook and interface mode.
  static constexpr int GRAB_BIT        = 0x04;       ///< A child with GRAB_BIT get exclusive focus
                                                     ///< for events
  static constexpr int ENABLED_BIT     = 0x10;       ///< Visibility and `onUpdate()` enabled.
  static constexpr int VISIBLE_BIT     = 0x20;       ///< Visible.

  // text alignment
  static constexpr int ALIGN_LEFT      = 0x00;
  static constexpr int ALIGN_RIGHT     = 0x01;
  static constexpr int ALIGN_HCENTRE   = 0x02;
  static constexpr int ALIGN_BOTTOM    = 0x00;
  static constexpr int ALIGN_TOP       = 0x04;
  static constexpr int ALIGN_VCENTRE   = 0x08;
  static constexpr int ALIGN_NONE      = ALIGN_LEFT | ALIGN_BOTTOM;
  static constexpr int ALIGN_CENTRE    = ALIGN_HCENTRE | ALIGN_VCENTRE;

protected:

  int          flags    = 0;

public:

  Area*        parent   = nullptr;
  DChain<Area> children;

  // absolute x and y, not relative to parent
  int          x        = 0;
  int          y        = 0;
  int          width;
  int          height;

protected:

  int          defaultX = 0;
  int          defaultY = 0;

protected:

  void updateChildren();
  // return true if event has been caught
  bool passMouseEvents();
  bool passKeyEvents();
  void drawChildren();

  virtual void onVisibilityChange(bool doShow);
  virtual void onRealign();
  virtual void onUpdate();
  // return true if event has been caught
  virtual bool onMouseEvent();
  virtual bool onKeyEvent();
  virtual void onDraw();

public:

  explicit Area(int width_, int height_);
  virtual ~Area();

  OZ_ALWAYS_INLINE
  bool isEnabled() const
  {
    return flags & ENABLED_BIT;
  }

  void enable(bool doEnable);

  OZ_ALWAYS_INLINE
  bool isVisible() const
  {
    return (flags & (ENABLED_BIT | VISIBLE_BIT)) == (ENABLED_BIT | VISIBLE_BIT);
  }

  void show(bool doShow);

  /**
   * Calculate global (x, y) for given relative rectangle coordinates/dimension.
   */
  Pos2 alignChild(int localChildX, int localChildY, int childWidth, int childHeight) const;

  /**
   * Fix position if parent has been resized.
   */
  void realign();

  /**
   * Move for `moveX` to the right and for `moveY` up.
   */
  void move(int dx, int dy);

  void add(Area* area, int localX, int localY);
  void remove(Area* area);
  void raise();
  void sink();

};

}
