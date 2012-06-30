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
 * @file client/MenuStage.hh
 */

#pragma once

#include "client/Stage.hh"

#include "client/ui/MainMenu.hh"

namespace oz
{
namespace client
{

class MenuStage : public Stage
{
  private:

    ui::MainMenu* mainMenu;

    bool showQuicksaved;
    bool showAutosaved;

  public:

    bool doExit;

    bool update() override;
    void present( bool isFull ) override;
    void wait( uint micros ) override;

    void load() override;
    void unload() override;

    void init() override;
    void free() override;

};

extern MenuStage menuStage;

}
}
