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

#include <client/ui/CreditsMenu.hh>

#include <client/Shape.hh>
#include <client/Camera.hh>
#include <client/Input.hh>
#include <client/ui/Mouse.hh>
#include <client/ui/Style.hh>
#include <client/ui/Button.hh>

namespace oz
{
namespace client
{
namespace ui
{

static void back(Button* sender)
{
  CreditsMenu* creditsMenu = static_cast<CreditsMenu*>(sender->parent);
  creditsMenu->parent->remove(creditsMenu);
}

void CreditsMenu::onRealign()
{
  width  = camera.width;
  height = camera.height;

  int nLabels = max(0, (height - 140) / stride);
  labels.resize(nLabels, true);

  for (int i = 0; i < nLabels; ++i) {
    int line = scroll + i;

    if (line < nLabels) {
      labels[i] = Text(0, 0, 0, ALIGN_HCENTRE, &style.sansFont, "");
    }
    else {
      labels[i] = Text(0, 0, 0, ALIGN_HCENTRE, &style.sansFont, "%s", lines[line - nLabels].c());
    }
  }
}

void CreditsMenu::onUpdate()
{
  if (direction == 0) {
    return;
  }

  bias += direction * 2;

  if (bias < 0 || bias >= stride) {
    int nEntries = labels.size() + lines.size();

    scroll = (scroll + direction + nEntries) % nEntries;
    bias   = direction < 0 ? bias + stride - 1 : 0;

    for (int i = 0; i < labels.size(); ++i) {
      int line = (scroll + i) % nEntries;

      if (line < labels.size()) {
        labels[i].setText("");
      }
      else {
        labels[i].setText("%s", lines[line - labels.size()].c());
      }
    }
  }
}

bool CreditsMenu::onMouseEvent()
{
  if (mouse.x < width - 240) {
    if (input.leftReleased) {
      direction = !direction;
    }
    else if (input.mouseW < 0.0f) {
      direction = +1;
    }
    else if (input.mouseW > 0.0f) {
      direction = -1;
    }
  }

  passMouseEvents();
  return true;
}

bool CreditsMenu::onKeyEvent()
{
  if (input.keys[Input::KEY_QUIT]) {
    parent->remove(this);
    return true;
  }
  else {
    return passKeyEvents();
  }
}

void CreditsMenu::onDraw()
{
  shape.colour(0.0f, 0.0f, 0.0f, 1.0f);
  shape.fill(width - 240, 0, 240, height - 40);

  for (int i = 0; i < labels.size(); ++i) {
    int x = (width - 240) / 2;
    int y = height - 60 - (i + 1)*stride + bias;

    labels[i].setPosition(x, y);
    labels[i].draw(this);
  }

  drawChildren();
}

CreditsMenu::CreditsMenu() :
  Area(camera.width, camera.height),
  stride(style.sansFont.height()), scroll(0), bias(0), direction(1)
{
  flags |= UPDATE_BIT;

  Button* backButton = new Button(OZ_GETTEXT("Back"), back, 200, 30);
  add(backButton, -20, 20);

  lines.add("OpenZone " OZ_VERSION);
  lines.add("");
  lines.add(OZ_GETTEXT("engine developer"));
  lines.add("Davorin Učakar");

  File creditsDir = "@credits";

  for (const File& creditsFile : creditsDir.list()) {
    Stream is = creditsFile.read();

    if (is.available() == 0) {
      OZ_ERROR("Failed to read '%s'", creditsFile.c());
    }

    lines.add("");
    lines.add("");
    lines.add("");
    lines.add("");
    lines.add("");
    lines.add(String::format(">> %s \"%s\" <<", OZ_GETTEXT("package"), creditsFile.baseName().c()));
    lines.add("");
    lines.add("");

    String contents = "";

    while (is.available() != 0) {
      contents += is.readLine() + "\n";
    }
    contents = lingua.get(contents);

    List<String> fileLines = contents.split('\n');
    lines.takeAll(fileLines.begin(), fileLines.size());

    if (!lines.isEmpty() && lines.last().isEmpty()) {
      lines.popLast();
    }
  }

  lines.add("");
  lines.add("");
  lines.add("");
  lines.add("");

  lines.add(OZ_GETTEXT("The following libraries may be bundled with the engine:"));
  lines.add(OZ_GETTEXT("(See doc/README.html for individual copyrights and licences.)"));
  lines.add("");
  lines.add("Assimp");
  lines.add("FreeImage");
  lines.add("FreeType");
  lines.add("libnoise");
  lines.add("libogg");
  lines.add("libpng");
  lines.add("libsquish");
  lines.add("libvorbis");
  lines.add("Lua");
  lines.add("OpenAL Soft");
  lines.add("PhysicsFS");
  lines.add("SDL");
  lines.add("SDL_ttf");
  lines.add("zlib");
}

}
}
}
