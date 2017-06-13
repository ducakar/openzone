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

#include <client/EditStage.hh>

#include <matrix/Synapse.hh>
#include <matrix/Matrix.hh>
#include <nirvana/Nirvana.hh>
#include <client/Context.hh>
#include <client/Loader.hh>
#include <client/Render.hh>
#include <client/Sound.hh>
#include <client/Camera.hh>
#include <client/Profile.hh>
#include <client/MenuStage.hh>
#include <client/Input.hh>
#include <client/ui/LoadingArea.hh>
#include <client/ui/StrategicArea.hh>
#include <client/ui/UI.hh>

namespace oz
{
namespace client
{

void EditStage::read()
{
  Log::print("Loading layout from '%s' ...", layoutFile.c());

  Json json;
  if (!json.load(layoutFile)) {
    OZ_ERROR("Reading saved layout '%s' failed", layoutFile.c());
  }

  Log::printEnd(" OK");

  matrix.read(json["matrix"]);
  camera.read(json["camera"]);

  json.clear(true);
}

void EditStage::write() const
{
  Json json(Json::OBJECT);

  json.add("matrix", matrix.write());
  json.add("camera", camera.write());

  Log::print("Saving layout to %s ...", layoutFile.c());

  if (!json.save(layoutFile)) {
    Log::printEnd(" Failed");
    System::bell();
  }
  else {
    Log::printEnd(" OK");
  }
}

void EditStage::auxMain(void*)
{
  editStage.auxRun();
}

void EditStage::auxRun()
{
  auxSemaphore.wait();

  while (isAuxAlive.load<ATOMIC_RELAXED>()) {
    /*
     * PHASE 2
     *
     * World is being updated, other threads should not access world structures here.
     */

    // update world
    matrix.update();

    mainSemaphore.post();
    auxSemaphore.wait();

    /*
     * PHASE 3
     *
     * Process AI, main thread renders world and plays sound.
     */

    // now synapse lists are not needed any more
    synapse.update();

    // we can now manipulate world from the main thread after synapse lists have been cleared
    // and nirvana is not accessing matrix any more
    mainSemaphore.post();
    auxSemaphore.wait();

    /*
     * PHASE 1
     *
     * Nothing, main thread may manipulate world here.
     */
  }
}

bool EditStage::update()
{
  mainSemaphore.wait();

  /*
   * PHASE 1
   *
   * UI update, world may be updated from the main thread during this phase.
   */

  orbis.resetLastIndices();

  if (input.keys[Input::KEY_QUICKSAVE] && !input.oldKeys[Input::KEY_QUICKSAVE]) {
    if (!layoutFile.isEmpty()) {
      write();
    }
  }
  if (input.keys[Input::KEY_QUICKLOAD] && !input.oldKeys[Input::KEY_QUICKLOAD]) {
    if (layoutFile.isFile()) {
      Stage::nextStage = this;
    }
  }

  if (input.keys[Input::KEY_DELETE] && !input.oldKeys[Input::KEY_DELETE]) {
    Struct* str = orbis.str(ui::ui.strategicArea->taggedStr);

    if (str != nullptr) {
      synapse.remove(str);
    }

    for (int i : ui::ui.strategicArea->taggedObjs) {
      Object* obj = orbis.obj(i);

      if (obj != nullptr) {
        synapse.remove(obj);
      }
    }
  }

  camera.prepare();

  auxSemaphore.post();

  /*
   * PHASE 2
   *
   * World is being updated in the auxiliary thread, any access of world structures might crash the
   * game.
   */

  context.updateLoad();
  loader.update();

  auxSemaphore.post();
  mainSemaphore.wait();

  /*
   * PHASE 3
   *
   * AI is processed in auxiliary thread here, world is rendered later in this phase in present().
   */

  camera.update();

  return !input.keys[Input::KEY_QUIT];
}

void EditStage::present(bool isFull)
{
  sound.play();
  render.update(Render::EFFECTS_BIT | (isFull ? Render::ORBIS_BIT | Render::UI_BIT : 0));
  sound.sync();
}

void EditStage::wait(Duration duration)
{
  Time::sleep(duration);
}

void EditStage::load()
{
  Log::println("[%s] Loading EditStage {", Time::local().toString().c());
  Log::indent();

  ui::mouse.isVisible = false;
  ui::ui.loadingScreen->status.setText("%s", OZ_GETTEXT("Loading ..."));
  ui::ui.loadingScreen->show(true);

  render.update(Render::UI_BIT);

  timer.reset();

  matrix.load();
  nirvana.load();

  sound.load();
  render.load();
  context.load();

  camera.reset();
  camera.setState(Camera::STRATEGIC);
  camera.strategic.hasBuildFrame = true;

  editFrame = new ui::EditFrame();
  ui::ui.root->add(editFrame, ui::Area::CENTRE, 8);

  if (layoutFile.isFile()) {
    read();
  }

  synapse.update();

  input.buttons = 0;
  input.currButtons = 0;

  camera.prepare();
  camera.update();

  ui::ui.showLoadingScreen(true);

  render.update(Render::ORBIS_BIT | Render::UI_BIT | Render::EFFECTS_BIT);

  loader.syncUpdate();
  loader.load();

  isAuxAlive.store<ATOMIC_RELAXED>(true);

  mainSemaphore.post();

  auxThread = Thread("aux", auxMain);

  ui::ui.showLoadingScreen(false);
  present(true);

  Log::unindent();
  Log::println("}");
}

void EditStage::unload()
{
  Log::println("[%s] Unloading EditStage {", Time::local().toString().c());
  Log::indent();

  ui::mouse.isVisible = false;
  ui::ui.loadingScreen->status.setText("%s", OZ_GETTEXT("Shutting down ..."));
  ui::ui.showLoadingScreen(true);

  loader.unload();

  render.update(Render::UI_BIT);

  isAuxAlive.store<ATOMIC_RELAXED>(false);

  auxSemaphore.post();
  mainSemaphore.wait();
  auxThread.join();

  ui::ui.root->remove(editFrame);
  editFrame = nullptr;

  camera.reset();

  context.unload();
  render.unload();
  sound.unload();

  nirvana.unload();
  matrix.unload();

  ui::ui.showLoadingScreen(false);

  Log::unindent();
  Log::println("}");
}

void EditStage::init()
{
  Log::println("Initialising EditStage {");
  Log::indent();

  matrix.init();
  nirvana.init();
  loader.init();
  profile.init();

  Log::unindent();
  Log::println("}");
}

void EditStage::destroy()
{
  Log::println("Destroying EditStage {");
  Log::indent();

  profile.destroy();
  loader.destroy();
  nirvana.destroy();
  matrix.destroy();

  layoutFile = "";

  Log::unindent();
  Log::println("}");
}

EditStage editStage;

}
}
