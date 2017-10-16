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

#include <client/ui/SettingsMenu.hh>

#include <client/Camera.hh>
#include <client/Shape.hh>
#include <client/Input.hh>
#include <client/Render.hh>
#include <client/ui/Style.hh>
#include <client/ui/Button.hh>
#include <client/ui/UI.hh>

namespace oz::client::ui
{

void SettingsMenu::back(Button* sender)
{
  SettingsMenu* settingsMenu = static_cast<SettingsMenu*>(sender->parent);
  settingsMenu->parent->remove(settingsMenu);
}

void SettingsMenu::apply(Button* sender)
{
  SettingsMenu* settingsMenu = static_cast<SettingsMenu*>(sender->parent);

  render.showAim         = settingsMenu->showAim->isChecked;
  render.showBounds      = settingsMenu->showBounds->isChecked;
  ui.showDebug           = settingsMenu->showDebug->isChecked;
  ui.showFPS             = settingsMenu->showFPS->isChecked;
  render.visibilityRange = settingsMenu->distance->value;

  settingsMenu->parent->remove(settingsMenu);
}

void SettingsMenu::openProfile(Button* sender)
{
  SettingsMenu* settingsMenu = static_cast<SettingsMenu*>(sender->parent);

  settingsMenu->profile->enable(true);
  settingsMenu->graphics->enable(false);
  settingsMenu->sound->enable(false);
  settingsMenu->debug->enable(false);
}
void SettingsMenu::openGraphics(Button* sender)
{
  SettingsMenu* settingsMenu = static_cast<SettingsMenu*>(sender->parent);

  settingsMenu->profile->enable(false);
  settingsMenu->graphics->enable(true);
  settingsMenu->sound->enable(false);
  settingsMenu->debug->enable(false);
}

void SettingsMenu::openSound(Button* sender)
{
  SettingsMenu* settingsMenu = static_cast<SettingsMenu*>(sender->parent);

  settingsMenu->profile->enable(false);
  settingsMenu->graphics->enable(false);
  settingsMenu->sound->enable(true);
  settingsMenu->debug->enable(false);
}

void SettingsMenu::openDebug(Button* sender)
{
  SettingsMenu* settingsMenu = static_cast<SettingsMenu*>(sender->parent);

  settingsMenu->profile->enable(false);
  settingsMenu->graphics->enable(false);
  settingsMenu->sound->enable(false);
  settingsMenu->debug->enable(true);
}

void SettingsMenu::onRealign()
{
  width  = camera.width;
  height = camera.height;
}

bool SettingsMenu::onMouseEvent()
{
  passMouseEvents();
  return true;
}

bool SettingsMenu::onKeyEvent()
{
  if (input.keys[Input::KEY_QUIT]) {
    parent->remove(this);
    return true;
  }
  else {
    return passKeyEvents();
  }
}

void SettingsMenu::onDraw()
{
  shape.colour(style.colours.menuStrip);
  shape.fill(width - 240, 0, 240, height - 40);

  drawChildren();
}

SettingsMenu::~SettingsMenu()
{}

SettingsMenu::SettingsMenu()
  : Area(camera.width, camera.height)
{
  Button* backButton     = new Button(OZ_GETTEXT("Back"),     back,         200, 30);
  Button* applyButton    = new Button(OZ_GETTEXT("Apply"),    apply,        200, 30);
  Button* profileButton  = new Button(OZ_GETTEXT("Profile"),  openProfile,  200, 30);
  Button* graphicsButton = new Button(OZ_GETTEXT("Graphics"), openGraphics, 200, 30);
  Button* soundButton    = new Button(OZ_GETTEXT("Sound"),    openSound,    200, 30);
  Button* debugButton    = new Button(OZ_GETTEXT("Debug"),    openDebug,    200, 30);

  add(profileButton,  -20, 240);
  add(graphicsButton, -20, 200);
  add(soundButton,    -20, 160);
  add(debugButton,    -20, 120);
  add(applyButton,    -20,  60);
  add(backButton,     -20,  20);

  profile  = new Area(width, height);
  graphics = new Area(width, height);
  sound    = new Area(width, height);
  debug    = new Area(width, height);

  showAim    = new CheckBox(OZ_GETTEXT("Aim cue"),       240, 20);
  showBounds = new CheckBox(OZ_GETTEXT("Object bounds"), 240, 20);
  showDebug  = new CheckBox(OZ_GETTEXT("Debug window"),  240, 20);
  showFPS    = new CheckBox(OZ_GETTEXT("FPS"),           240, 20);

  debug->add(showAim,    60, height - 160);
  debug->add(showBounds, 60, height - 190);
  debug->add(showDebug,  60, height - 220);
  debug->add(showFPS,    60, height - 250);

  showAim->isChecked    = render.showAim;
  showBounds->isChecked = render.showBounds;
  showDebug->isChecked  = ui.showDebug;
  showFPS->isChecked    = ui.showFPS;

  distance = new Slider(100.0f, 800.0f, 50.0f, render.visibilityRange, 240, 20);

  graphics->add(distance, 60, height - 160);

  add(profile,  0, 0);
  add(graphics, 0, 0);
  add(sound,    0, 0);
  add(debug,    0, 0);

  profile->enable(true);
  graphics->enable(false);
  sound->enable(false);
  debug->enable(false);
}

}
