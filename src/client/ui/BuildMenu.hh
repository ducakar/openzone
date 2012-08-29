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
 * @file client/ui/BuildMenu.hh
 */

#pragma once

#include <client/ui/Frame.hh>
#include <client/ui/Button.hh>

namespace oz
{
namespace client
{
namespace ui
{

class BuildButton : public Button
{
  public:

    String className;

    explicit BuildButton( const char* className, Callback* callback, int width, int height );
    ~BuildButton() override;

};

class BuildMenu : public Frame
{
  private:

    static void createObject( Button* button );

  public:

    BuildMenu();
    ~BuildMenu() override;

};

}
}
}
