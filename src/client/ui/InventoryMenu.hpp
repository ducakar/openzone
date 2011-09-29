/*
 *  InventoryMenu.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/ui/Frame.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  class InventoryMenu : public Frame
  {
    private:

      static const int   ICON_SIZE   = 32;
      static const int   SLOT_SIZE   = 64;
      static const float SLOT_DIMF;
      static const int   COLS        = 8;
      static const int   ROWS        = 2;
      static const int   FOOTER_SIZE = 32;

      const InventoryMenu* const master;

      Label  itemDesc;

      int    tagged;
      int    scroll;

      uint   scrollUpTexId;
      uint   scrollDownTexId;
      uint   useTexId;
      uint   equipTexId;
      uint   unequipTexId;

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
