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
 * @file client/MenuStage.hh
 */

#pragma once

#include <client/Stage.hh>

#include <client/ui/MainMenu.hh>

namespace oz::client
{

class MenuStage : public Stage
{
private:

  ui::MainMenu* mainMenu;

public:

  bool doExit;

  bool update() override;
  void present(bool isFull) override;
  void wait(Duration duration) override;

  void load() override;
  void unload() override;

  void init() override;
  void destroy() override;

};

extern MenuStage menuStage;

}
