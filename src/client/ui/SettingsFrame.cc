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
 * @file client/ui/SettingsFrame.cc
 */

#include <client/ui/SettingsFrame.hh>

#include <common/Lingua.hh>
#include <client/Camera.hh>
#include <client/ui/Style.hh>
#include <client/ui/Button.hh>
#include <client/ui/CheckBox.hh>

namespace oz
{
namespace client
{
namespace ui
{

static void closeFrame(Button* sender)
{
  SettingsFrame* settings = static_cast<SettingsFrame*>(sender->parent);
  settings->parent->remove(settings);
}

void SettingsFrame::onDraw()
{
  Frame::onDraw();

  message.draw(this);
}

SettingsFrame::SettingsFrame() :
  Frame(400, 40 + 8 * style.fonts[Font::SANS].height, OZ_GETTEXT("Settings")),
  message(4, 24, -HEADER_SIZE - 4, Area::ALIGN_NONE, Font::SANS, "")
{
  x = (camera.width - width) / 2;
  y = (camera.height - height) / 2;

  add(new CheckBox("Mouse smoothing", width - 20, 20), 10, 40);
  add(new Button(OZ_GETTEXT("Close"), closeFrame, 80, 25), -4, 4);
}

}
}
}
