/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file client/ui/SettingsFrame.hpp
 */

#pragma once

#include "client/ui/Frame.hpp"
#include "client/ui/Button.hpp"
#include "client/ui/Text.hpp"

namespace oz
{
namespace client
{
namespace ui
{

class SettingsFrame : public Frame
{
  private:

    Text    message;
    Button* close;

  protected:

    virtual void onDraw();

  public:

    SettingsFrame();

};

}
}
}
