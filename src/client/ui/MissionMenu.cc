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

#include <client/ui/MissionMenu.hh>

#include <client/Camera.hh>
#include <client/Shader.hh>
#include <client/Shape.hh>
#include <client/GameStage.hh>
#include <client/Input.hh>
#include <client/ui/Mouse.hh>
#include <client/ui/Style.hh>

namespace oz::client::ui
{

static void back(Button* sender)
{
  MissionMenu* missionMenu = static_cast<MissionMenu*>(sender->parent);
  missionMenu->parent->remove(missionMenu);
}

bool MissionButton::onMouseEvent()
{
  int selection = missionMenu->scroll + index;

  if (missionMenu->selection != selection) {
    missionMenu->selection = selection;

    missionMenu->description.setText("%s", missionMenu->missions[selection].description.c());
    missionMenu->imageId = missionMenu->missions[selection].imageId;
  }

  return Button::onMouseEvent();
}

MissionButton::MissionButton(const char* text, Callback* callback, MissionMenu* missionMenu_,
                             int index_, int width_, int height_)
  : Button(text, callback, width_, height_), missionMenu(missionMenu_), index(index_)
{}

void MissionMenu::loadMission(Button* sender)
{
  MissionButton* button      = static_cast<MissionButton*>(sender);
  MissionMenu*   missionMenu = static_cast<MissionMenu*>(sender->parent);

  Stage::nextStage    = &gameStage;
  gameStage.stateFile = "";
  gameStage.mission   = missionMenu->missions[missionMenu->scroll + button->index].name;
}

void MissionMenu::onRealign()
{
  width       = camera.width;
  height      = camera.height;

  nSelections = clamp(missions.size(), 0, (height - 150) / 40);
  selection   = -1;
  scroll      = 0;

  imageX      = 20;
  imageY      = 100 + 8 * style.sansFont.height();
  imageWidth  = width - 280;
  imageHeight = height - 20 - imageY;

  description.setWidth(width - 320);

  if (selection != -1) {
    description.setText("%s", missions[selection].description.c());
  }

  float aspect = float(imageWidth) / float(imageHeight);
  if (aspect < 16.0f / 9.0f) {
    imageHeight = int(float(imageWidth) / 16.0f * 9.0f);
  }
  else if (aspect > 16.0f / 9.0f) {
    imageWidth = int(float(imageHeight) / 9.0f * 16.0f);
    imageX = (width - 240 - imageWidth) / 2;
  }

  while (children.first() != children.last()) {
    remove(children.first());
  }

  for (int i = 0; i < nSelections; ++i) {
    Button* missionButton = new MissionButton(missions[i].title, loadMission, this, i, 200, 30);
    add(missionButton, -20, -(i + 2) * 40);
  }
}

bool MissionMenu::onMouseEvent()
{
  if (mouse.x >= width - 240 && input.mouseW != 0.0f) {
    if (input.mouseW < 0.0f) {
      scroll = min(scroll + 1, missions.size() - nSelections);
    }
    else {
      scroll = max(scroll - 1, 0);
    }

    for (auto i = children.begin(); &*i != children.last(); ++i) {
      MissionButton* button = static_cast<MissionButton*>(&*i);

      button->setText(missions[scroll + button->index].title);
    }
  }

  passMouseEvents();
  return true;
}

bool MissionMenu::onKeyEvent()
{
  if (input.keys[Input::KEY_QUIT]) {
    parent->remove(this);
    return true;
  }
  else {
    return passKeyEvents();
  }
}

void MissionMenu::onDraw()
{
  shape.colour(style.colours.menuStrip);
  shape.fill(width - 240, 0, 240, height - 40);

  shape.colour(1.0f, 1.0f, 1.0f, 1.0f);

  if (scroll > 0) {
    glBindTexture(GL_TEXTURE_2D, style.images.scrollUp);
    shape.fill(width - 128, height - 32, 16, 16);
  }
  if (scroll < missions.size() - nSelections) {
    glBindTexture(GL_TEXTURE_2D, style.images.scrollDown);
    shape.fill(width - 128, height - nSelections * 40 - 54, 16, 16);
  }

  if (imageId != 0) {
    glBindTexture(GL_TEXTURE_2D, imageId);
    shape.fill(imageX, imageY, imageWidth, imageHeight);
  }

  glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);

  description.draw(this);

  drawChildren();
}

MissionMenu::~MissionMenu()
{
  for (const MissionInfo& mi : missions) {
    glDeleteTextures(1, &mi.imageId);
  }

  OZ_GL_CHECK_ERROR();
}

MissionMenu::MissionMenu()
  : Area(camera.width, camera.height),
    description(40, 190, width - 320, ALIGN_TOP, &style.sansFont, "")
{
  Button* backButton = new Button(OZ_GETTEXT("Back"), back, 200, 30);
  add(backButton, -20, 20);

  File missionRootDir = "@mission";

  for (const File& missionDir : missionRootDir.list()) {
    File descriptionFile = missionDir / "description.json";

    Json descriptionConfig;
    if (!descriptionConfig.load(descriptionFile)) {
      continue;
    }

    String missionName = missionDir.baseName();

    Lingua lingua;
    lingua.initMission(missionName);

    const char* missionTitle       = lingua.get(descriptionConfig["title"].get(missionName));
    const char* missionDescription = lingua.get(descriptionConfig["description"].get(""));
    uint        missionImageId     = 0;

    MainCall() << [&]
    {
      glGenTextures(1, &missionImageId);
      glBindTexture(GL_TEXTURE_2D, missionImageId);
      GL::textureDataFromFile(missionDir / "description.dds");
    };

    missions.add(MissionInfo{missionName, missionTitle, missionDescription, missionImageId});

    lingua.clear();
  }
}

}
