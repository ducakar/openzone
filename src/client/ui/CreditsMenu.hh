/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
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
 * @file client/ui/CreditsMenu.hh
 */

#pragma once

#include <client/ui/Text.hh>
#include <client/ui/Area.hh>

namespace oz::client::ui
{

class CreditsMenu : public Area
{
private:

  List<String> lines;
  List<Text>   labels;
  const int    stride;
  int          scroll;
  int          bias;
  int          direction;

protected:

  void onRealign() override;
  void onUpdate() override;
  bool onMouseEvent() override;
  bool onKeyEvent() override;
  void onDraw() override;

public:

  CreditsMenu();

};

}
