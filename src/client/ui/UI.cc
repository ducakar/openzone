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

#include <client/ui/UI.hh>

#include <client/Camera.hh>
#include <client/Input.hh>
#include <client/Context.hh>
#include <client/Shape.hh>
#include <client/ui/Style.hh>
#include <client/ui/LoadingArea.hh>
#include <client/ui/StrategicArea.hh>
#include <client/ui/HudArea.hh>
#include <client/ui/QuestFrame.hh>
#include <client/ui/GalileoFrame.hh>
#include <client/ui/MusicPlayer.hh>
#include <client/ui/Inventory.hh>
#include <client/ui/InfoFrame.hh>
#include <client/ui/BuildFrame.hh>
#include <client/ui/DebugFrame.hh>

namespace oz
{
namespace client
{
namespace ui
{

UI::UI() :
  fps(0.0f), fpsLabel(nullptr), isFreelook(false), showFPS(false), showDebug(false),
  isVisible(true), root(nullptr), loadingScreen(nullptr), hudArea(nullptr), strategicArea(nullptr),
  questFrame(nullptr), galileoFrame(nullptr), musicPlayer(nullptr), inventory(nullptr),
  buildFrame(nullptr), debugFrame(nullptr)
{}

void UI::showLoadingScreen(bool doShow)
{
  loadingScreen->raise();
  loadingScreen->show(doShow);
}

void UI::update()
{
  if (mouse.isVisible == isFreelook) {
    isFreelook = !mouse.isVisible;

    for (Area& area : root->children) {
      if (!(area.flags & Area::PINNED_BIT)) {
        area.show(mouse.isVisible);
      }
    }
  }

  root->updateChildren();

  if (!isFreelook) {
    if (input.isKeyPressed || input.isKeyReleased) {
      root->passKeyEvents();
    }
    if (isVisible) {
      root->passMouseEvents();
    }
  }
}

void UI::draw()
{
  OZ_NACL_IS_MAIN(true);

  if (!isVisible) {
    return;
  }

  glViewport(0, 0, camera.width, camera.height);
  glClear(GL_DEPTH_BUFFER_BIT);

  shape.bind();

  tf.ortho(camera.width, camera.height);
  tf.camera = Mat4::ID;
  tf.colour = Mat4::ID;

  // set shaders
  shader.setAmbientLight(Vec4(0.5f, 0.5f, 0.5f, 1.0f));
  shader.setCaelumLight(Vec3(-0.4851f, 0.4851f, 0.7276f), Vec4(0.5f, 0.5f, 0.5f, 1.0f));

  for (int i = 0; i < liber.shaders.length(); ++i) {
    shader.program(i);

    tf.applyCamera();
    shader.updateLights();

    glUniform1f(uniform.fog_distance2, Math::INF);
    glUniform4f(uniform.wind, 0.0f, 0.0f, 0.0f, 0.0f);
  }

  shader.program(shader.plain);

  root->drawChildren();
  mouse.draw();

  if (showFPS) {
    if (timer.frameTicks != 0) {
      fps = Math::mix(fps, 1.0f / timer.frameTime, 0.04f);
    }

    fpsLabel->setText("%.1f", fps);
    fpsLabel->draw(root);
  }

  shape.unbind();

  OZ_GL_CHECK_ERROR();
}

void UI::load()
{
  fps           = 1.0f / Timer::TICK_TIME;
  isFreelook    = false;
  isVisible     = true;

  hudArea       = new HudArea();
  strategicArea = new StrategicArea();
  questFrame    = new QuestFrame();
  galileoFrame  = new GalileoFrame();
  musicPlayer   = new MusicPlayer();
  inventory     = new Inventory();
  infoFrame     = new InfoFrame();
  buildFrame    = new BuildFrame();
  debugFrame    = showDebug ? new DebugFrame() : nullptr;

  root->add(hudArea, 0, 0);
  root->add(strategicArea, 0, 0);
  root->add(questFrame, Area::CENTRE, -8);
  root->add(galileoFrame, 8, -8);
  root->add(musicPlayer, 8, -16 - galileoFrame->height);
  root->add(inventory, Area::CENTRE, 8);
  root->add(infoFrame, -8, -8);
  root->add(buildFrame, -8, -8);

  if (showDebug) {
    root->add(debugFrame, Area::CENTRE, -16 - questFrame->height);
  }

  hudArea->enable(false);
  strategicArea->enable(false);
  questFrame->enable(false);
  inventory->enable(false);
  infoFrame->enable(false);
  buildFrame->enable(false);

  loadingScreen->raise();
}

void UI::unload()
{
  if (debugFrame != nullptr) {
    root->remove(debugFrame);
    debugFrame = nullptr;
  }
  if (buildFrame != nullptr) {
    root->remove(buildFrame);
    buildFrame = nullptr;
  }
  if (infoFrame != nullptr) {
    root->remove(infoFrame);
    infoFrame = nullptr;
  }
  if (inventory != nullptr) {
    root->remove(inventory);
    inventory = nullptr;
  }
  if (musicPlayer != nullptr) {
    root->remove(musicPlayer);
    musicPlayer = nullptr;
  }
  if (galileoFrame != nullptr) {
    root->remove(galileoFrame);
    galileoFrame = nullptr;
  }
  if (questFrame != nullptr) {
    root->remove(questFrame);
    questFrame = nullptr;
  }
  if (strategicArea != nullptr) {
    root->remove(strategicArea);
    strategicArea = nullptr;
  }
  if (hudArea != nullptr) {
    root->remove(hudArea);
    hudArea = nullptr;
  }
}

void UI::init()
{
  fps        = 1.0f / Timer::TICK_TIME;
  isFreelook = false;
  showFPS    = config.include("ui.showFPS",   false).get(false);
  showDebug  = config.include("ui.showDebug", false).get(false);
  isVisible     = true;

  style.init();
  mouse.init();

  root = new Area(camera.width, camera.height);
  loadingScreen = new LoadingArea();

  if (showFPS) {
    fpsLabel = new Text(-4, -4, 0, Area::ALIGN_RIGHT | Area::ALIGN_TOP, &style.monoFont, "");
  }

  root->add(loadingScreen, 0, 0);
}

void UI::destroy()
{
  delete fpsLabel;
  delete root;

  root     = nullptr;
  fpsLabel = nullptr;

  mouse.destroy();
  style.destroy();

  context.clearSounds();
}

UI ui;

}
}
}
