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
 * @file client/ui/EditFrame.hh
 */

#pragma once

#include <client/ui/Bar.hh>
#include <client/ui/ModelField.hh>
#include <client/ui/Frame.hh>

namespace oz::client::ui
{

class EditFrame : public Frame
{
private:

  static constexpr int COLS          = 8;
  static constexpr int FOOTER_SIZE   = 32;
  static constexpr int ICON_SIZE     = 32;
  static constexpr int SLOT_SIZE     = 68;
  static constexpr int SLOT_OBJ_DIM  = SLOT_SIZE / 2;
  static constexpr int SINGLE_HEIGHT = FOOTER_SIZE + SLOT_SIZE;

  Object*     owner = nullptr;

  Bar         lifeBar;
  Bar         statusBar;

  ModelField* ownerModels[COLS];

  Text        itemDesc;

  int         taggedItemIndex = -1;
  int         scrollOwner     = 0;

private:

  static void itemCallback(ModelField* sender, bool isClicked);

  void updateReferences();
  void handleScroll(const Object* container, int* scroll);
  void drawComponent(int componentHeight, const Object* container, const Dynamic* taggedItem,
                     int scroll);

protected:

  void onVisibilityChange(bool doShow) override;
  bool onMouseEvent() override;
  void onUpdate() override;
  void onDraw() override;

public:

  EditFrame();

};

}
