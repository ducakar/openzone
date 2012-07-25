/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file client/ui/Inventory.hh
 */

#pragma once

#include "client/ui/Frame.hh"

namespace oz
{
namespace client
{
namespace ui
{

class Inventory : public Frame
{
  private:

    static const int COLS          = 8;
    static const int ROWS          = 1;
    static const int PADDING_SIZE  = 2;
    static const int FOOTER_SIZE   = 32;
    static const int ICON_SIZE     = 32;
    static const int SLOT_SIZE     = 68;
    static const int SLOT_OBJ_DIM  = ( SLOT_SIZE - 2*PADDING_SIZE ) / 2;
    static const int SINGLE_HEIGHT = FOOTER_SIZE + ROWS * SLOT_SIZE;

    const Object* owner;
    const Object* other;

    uint  scrollUpTexId;
    uint  scrollDownTexId;
    uint  useTexId;
    uint  equipTexId;
    uint  unequipTexId;

    Label itemDesc;

    int   cachedContainerIndex;
    int   cachedTaggedItemIndex;

    int   taggedOwner;
    int   taggedOther;
    int   scrollOwner;
    int   scrollOther;

    bool  isMouseOver;

  private:

    void updateReferences();
    void handleComponent( int height, const Object* container, int* tagged, int* scroll );
    void drawComponent( int height, const Object* container, int tagged, int scroll );

  protected:

    void onVisibilityChange( bool doShow ) override;
    bool onMouseEvent() override;
    void onUpdate() override;
    void onDraw() override;

  public:

    Inventory();
    ~Inventory() override;

};

}
}
}
