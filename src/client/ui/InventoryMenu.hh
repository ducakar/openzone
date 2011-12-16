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
 * @file client/ui/InventoryMenu.hh
 */

#pragma once

#include "client/ui/Frame.hh"

namespace oz
{
namespace client
{
namespace ui
{

class InventoryMenu : public Frame
{
  private:

    static const int   ICON_SIZE    = 32;
    static const int   SLOT_SIZE    = 68;
    static const float SLOT_OBJ_DIMF;
    static const int   COLS         = 8;
    static const int   ROWS         = 1;
    static const int   PADDING_SIZE = 2;
    static const int   FOOTER_SIZE  = 32;

    const InventoryMenu* const master;

    uint   scrollUpTexId;
    uint   scrollDownTexId;
    uint   useTexId;
    uint   equipTexId;
    uint   unequipTexId;

    Label  itemDesc;

    int    tagged;
    int    scroll;

  protected:

    virtual void onVisibilityChange();
    virtual bool onMouseEvent();
    virtual void onDraw();

  public:

    InventoryMenu( const InventoryMenu* master );
    virtual ~InventoryMenu();

};

}
}
}
