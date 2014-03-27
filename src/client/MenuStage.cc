/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file client/MenuStage.cc
 */

#include <client/MenuStage.hh>

#include <client/Camera.hh>
#include <client/Render.hh>
#include <client/Sound.hh>
#include <client/Input.hh>
#include <client/ui/UI.hh>

namespace oz
{
namespace client
{

bool MenuStage::update()
{
  camera.prepare();
  camera.update();

  return !doExit;
}

void MenuStage::present( bool isFull )
{
  // We don't want to hide UI or mouse cursor in the menu.
  ui::ui.doShow = true;

  sound.play();
  MainCall() << [&]() {
    render.update( isFull ? Render::DRAW_UI_BIT : 0 );
  };
  sound.sync();
}

void MenuStage::wait( uint micros )
{
  Time::usleep( micros );
}

void MenuStage::load()
{
  input.buttons     = 0;
  input.currButtons = 0;

  MainCall() << [&]() {
    mainMenu = new ui::MainMenu();
    ui::ui.root->add( mainMenu, 0, 0 );
    ui::ui.showLoadingScreen( false );
    ui::mouse.doShow = true;

    render.update( Render::DRAW_UI_BIT );
  };
}

void MenuStage::unload()
{
  MainCall() << [&]() {
    ui::mouse.doShow = false;
    ui::ui.root->remove( mainMenu );
  };
}

void MenuStage::init()
{
  doExit = false;
}

void MenuStage::destroy()
{}

MenuStage menuStage;

}
}
