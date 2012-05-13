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
 * @file client/MenuStage.cc
 */

#include "stable.hh"

#include "client/MenuStage.hh"

#include "client/GameStage.hh"
#include "client/Render.hh"
#include "client/Sound.hh"

namespace oz
{
namespace client
{

MenuStage menuStage;

bool MenuStage::update()
{
  ui::ui.update();

  return !doExit;
}

void MenuStage::present( bool )
{
  render.draw( Render::DRAW_UI_BIT );
  sound.play();
  render.swap();
  sound.sync();
}

void MenuStage::wait( uint micros )
{
  Time::usleep( micros );
}

void MenuStage::load()
{
  ui::mouse.buttons     = 0;
  ui::mouse.currButtons = 0;

  ui::Area* area = new ui::MainMenu();
  ui::ui.root->add( area );

  ui::ui.showLoadingScreen( false );
  ui::mouse.doShow = true;

  render.draw( Render::DRAW_UI_BIT );
  render.draw( Render::DRAW_UI_BIT );
  render.swap();
}

void MenuStage::unload()
{
  ui::mouse.doShow = false;
}

void MenuStage::init()
{
  doExit = false;
}

void MenuStage::free()
{}

}
}
