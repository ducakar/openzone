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

#include <client/ui/InfoFrame.hh>

#include <nirvana/Nirvana.hh>
#include <client/Camera.hh>
#include <client/ui/Style.hh>

namespace oz::client::ui
{

const Device* const* InfoFrame::updateReferences()
{
  if (camera.state != Camera::UNIT || camera.botObj == nullptr || camera.object == -1) {
    lastId = -1;
    return nullptr;
  }

  const Device* const* device = nirvana.devices.find(camera.object);
  if (device == nullptr) {
    lastId = -1;
  }
  return device;
}

void InfoFrame::onVisibilityChange(bool)
{
  lastId = -1;
}

void InfoFrame::onUpdate()
{
  const Device* const* device = updateReferences();

  show(device != nullptr);
}

void InfoFrame::onDraw()
{
  const Device* const* device = updateReferences();
  if (device == nullptr) {
    return;
  }

  if (lastId != camera.object) {
    lastId = camera.object;

    const Bot* tagged = static_cast<const Bot*>(camera.objectObj);

    text.setText("%s", (*device)->getMemo());

    if (tagged->flags & Object::BOT_BIT) {
      title.setText("%s (%s)", tagged->name.c(), tagged->clazz->title.c());
    }
    else {
      title.setText("%s", tagged->clazz->title.c());
    }
  }

  Frame::onDraw();
  text.draw(this);
}

InfoFrame::InfoFrame()
  : Frame(320, 12 + 30 * style.sansFont.height(), ""),
    text(6, -HEADER_SIZE - 4, 308, Area::ALIGN_TOP, &style.sansFont, "")
{
  flags |= PINNED_BIT;
}

}
