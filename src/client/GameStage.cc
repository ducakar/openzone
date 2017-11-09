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

#include <client/GameStage.hh>

#include <matrix/Synapse.hh>
#include <matrix/Matrix.hh>
#include <nirvana/Nirvana.hh>
#include <client/Context.hh>
#include <client/Loader.hh>
#include <client/Render.hh>
#include <client/Sound.hh>
#include <client/Network.hh>
#include <client/Camera.hh>
#include <client/LuaClient.hh>
#include <client/Profile.hh>
#include <client/MenuStage.hh>
#include <client/Input.hh>
#include <client/ui/QuestFrame.hh>
#include <client/ui/LoadingArea.hh>
#include <client/ui/UI.hh>

namespace oz::client
{

void GameStage::saveMain(void*)
{
  Log::print("Saving state to %s ...", gameStage.saveFile.c());

  if (!gameStage.saveFile.write(gameStage.saveStream.compress())) {
    Log::printEnd(" Failed");
    System::bell();
  }
  else {
    Log::printEnd(" OK");
  }

  gameStage.saveStream.free();
  gameStage.saveFile = "";
}

void GameStage::read()
{
  Log::print("Loading state from '%s' ...", stateFile.c());

  Stream is(0);
  if (!stateFile.read(&is)) {
    OZ_ERROR("Reading saved state '%s' failed", stateFile.c());
  }

  is = is.decompress();

  Log::printEnd(" OK");

  matrix.read(&is);
  nirvana.read(&is);

  Log::println("Reading Client {");
  Log::indent();

  camera.read(&is);
  luaClient.read(&is);

  OZ_ASSERT(is.available() == 0);

  Log::unindent();
  Log::println("}");
}

void GameStage::write()
{
  if (saveThread.isValid()) {
    saveThread.join();
  }

  OZ_ASSERT(saveStream.capacity() == 0);

  matrix.write(&saveStream);
  nirvana.write(&saveStream);

  camera.write(&saveStream);

  luaClient.write(&saveStream);

  saveFile   = stateFile;
  saveThread = Thread("save", saveMain);
}

void GameStage::auxMain(void*)
{
  gameStage.auxRun();
}

void GameStage::auxRun()
{
  auxSemaphore.wait();

  while (isAuxAlive.load<RELAXED>()) {
    /*
     * PHASE 2
     *
     * World is being updated, other threads should not access world structures here.
     */

    Instant beginInstant = Instant::now();

    network.update();

    // update world
    matrix.update();

    matrixDuration += Instant::now() - beginInstant;

    mainSemaphore.post();
    auxSemaphore.wait();

    /*
     * PHASE 3
     *
     * Process AI, main thread renders world and plays sound.
     */

    beginInstant = Instant::now();

    // sync nirvana
    nirvana.sync();

    // now synapse lists are not needed any more
    synapse.update();

    // update minds
    nirvana.update();

    nirvanaDuration += Instant::now() - beginInstant;

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

bool GameStage::update()
{
  mainSemaphore.wait();

  /*
   * PHASE 1
   *
   * UI update, world may be updated from the main thread during this phase.
   */

  Instant beginInstant = Instant::now();

  if (input.keys[Input::KEY_QUIT]) {
    Stage::nextStage = &menuStage;
  }

  ++autosaveTicks;

  if (autosaveTicks > AUTOSAVE_INTERVAL) {
    autosaveTicks = 0;

    stateFile = autosaveFile;
    write();
    stateFile = "";
  }

  if (input.keys[Input::KEY_QUICKSAVE] && !input.oldKeys[Input::KEY_QUICKSAVE]) {
    stateFile = quicksaveFile;
    write();
    stateFile = "";
  }
  else if (input.keys[Input::KEY_QUICKLOAD] && !input.oldKeys[Input::KEY_QUICKLOAD]) {
    if (quicksaveFile.isRegular()) {
      stateFile = quicksaveFile;
      Stage::nextStage = this;
    }
  }
  else if (input.keys[Input::KEY_AUTOLOAD] && !input.oldKeys[Input::KEY_AUTOLOAD]) {
    if (autosaveFile.isRegular()) {
      stateFile = autosaveFile;
      Stage::nextStage = this;
    }
  }

  camera.prepare();

  luaClient.update();

  uiDuration += Instant::now() - beginInstant;

  auxSemaphore.post();

  /*
   * PHASE 2
   *
   * World is being updated in the auxiliary thread, any access of world structures might crash the
   * game.
   */

  beginInstant = Instant::now();

  context.updateLoad();
  loader.update();

  loaderDuration += Instant::now() - beginInstant;

  auxSemaphore.post();
  mainSemaphore.wait();

  /*
   * PHASE 3
   *
   * AI is processed in auxiliary thread here, world is rendered later in this phase in present().
   */

  camera.update();

  return true;
}

void GameStage::present(bool isFull)
{
  Instant beginInstant   = Instant::now();
  Instant currentInstant;

  sound.play();
  render.update(Render::EFFECTS_BIT | (isFull ? Render::ORBIS_BIT | Render::UI_BIT : 0));

  sound.sync();

  currentInstant = Instant::now();
  presentDuration += currentInstant - beginInstant;
}

void GameStage::wait(Duration duration)
{
  sleepDuration += duration;

  Thread::sleepFor(duration);
}

void GameStage::load()
{
  Log::println("[%s] Loading GameStage {", Time::local().toString().c());
  Log::indent();

  Instant beginInstant = Instant::now();

  ui::mouse.isVisible = false;
  ui::ui.loadingScreen->status.setText("%s", OZ_GETTEXT("Loading ..."));
  ui::ui.loadingScreen->show(true);

  render.update(Render::UI_BIT);

  timer.reset();

  sleepDuration   = Duration::ZERO;
  uiDuration      = Duration::ZERO;
  loaderDuration  = Duration::ZERO;
  presentDuration = Duration::ZERO;
  matrixDuration  = Duration::ZERO;
  nirvanaDuration = Duration::ZERO;

  network.connect();

  matrix.load();
  nirvana.load();

  luaClient.init();

  MainCall() << []
  {
    sound.load();
    render.load();
    context.load();
  };

  camera.reset();
  camera.setState(Camera::STRATEGIC);

  if (stateFile.isRegular()) {
    read();
  }
  else {
    Log::println("Initialising new world");

    Log::println("Loading Client {");
    Log::indent();

    luaClient.create(mission);

    Log::unindent();
    Log::println("}");
  }

  stateFile = "";

  nirvana.sync();
  synapse.update();

  input.buttons = 0;
  input.currButtons = 0;

  camera.prepare();
  camera.update();

  ui::ui.questFrame->enable(true);

  startTicks = timer.ticks;

  ui::ui.showLoadingScreen(true);

  render.update(Render::ORBIS_BIT | Render::UI_BIT | Render::EFFECTS_BIT);

  loader.syncUpdate();
  loader.load();

  isAuxAlive.store<RELAXED>(true);

  mainSemaphore.post();

  auxThread = Thread("aux", auxMain);

  ui::ui.showLoadingScreen(false);
  present(true);

  loadingDuration = Instant::now() - beginInstant;
  autosaveTicks = 0;

  Log::unindent();
  Log::println("}");
}

void GameStage::unload()
{
  Log::println("[%s] Unloading GameStage {", Time::local().toString().c());
  Log::indent();

  ui::mouse.isVisible = false;
  ui::ui.loadingScreen->status.setText("%s", OZ_GETTEXT("Shutting down ..."));
  ui::ui.showLoadingScreen(true);

  render.update(Render::UI_BIT);

  if (saveThread.isValid()) {
    saveThread.join();
  }

  loader.unload();

  isAuxAlive.store<RELAXED>(false);

  auxSemaphore.post();
  mainSemaphore.wait();
  auxThread.join();

  uint64   ticks                 = timer.ticks - startTicks;
  Duration soundMicros           = sound.effectsDuration + sound.musicDuration;
  Duration renderMicros          = render.prepareDuration + render.caelumDuration +
                                   render.terraDuration + render.meshesDuration +
                                   render.miscDuration + render.postprocessDuration +
                                   render.uiDuration + render.swapDuration;
  float    sleepTime             = sleepDuration.t();
  float    uiTime                = uiDuration.t();
  float    loaderTime            = loaderDuration.t();
  float    presentTime           = presentDuration.t();
  float    soundTime             = soundMicros.t();
  float    soundEffectsTime      = sound.effectsDuration.t();
  float    soundMusicTime        = sound.musicDuration.t();
  float    renderTime            = renderMicros.t();
  float    renderPrepareTime     = render.prepareDuration.t();
  float    renderCaelumTime      = render.caelumDuration.t();
  float    renderTerraTime       = render.terraDuration.t();
  float    renderMeshesTime      = render.meshesDuration.t();
  float    renderMiscTime        = render.miscDuration.t();
  float    renderPostprocessTime = render.postprocessDuration.t();
  float    renderUITime          = render.uiDuration.t();
  float    renderSwapTime        = render.swapDuration.t();
  float    matrixTime            = matrixDuration.t();
  float    nirvanaTime           = nirvanaDuration.t();
  float    loadingTime           = loadingDuration.t();
  float    runTime               = timer.runDuration.t();
  float    gameTime              = timer.time.t();
  float    droppedTime           = (timer.runDuration - timer.time).t();
  uint64   nFrameDrops           = ticks - timer.nFrames;
  float    frameDropRate         = float(ticks - timer.nFrames) / float(ticks);

  if (stateFile.isEmpty()) {
    stateFile = autosaveFile;
    write();
    saveThread.join();
    stateFile = "";
  }

  profile.save();

  ui::ui.questFrame->enable(false);

  camera.reset();

  MainCall() << []
  {
    context.unload();
    render.unload();
    sound.unload();
  };

  luaClient.destroy();

  nirvana.unload();
  matrix.unload();

  network.disconnect();

  ui::ui.showLoadingScreen(false);

  Log::println("Time statistics {");
  Log::indent();
  Log::println("loading time          %8.2f s",    loadingTime);
  Log::println("run time              %8.2f s",    runTime);
  Log::println("game time             %8.2f s",    gameTime);
  Log::println("dropped time          %8.2f s",    droppedTime);
  Log::println("optimal tick/frame rate %6.2f Hz", 1.0f / Timer::TICK_TIME);
  Log::println("tick rate in run time   %6.2f Hz", float(ticks) / runTime);
  Log::println("frame rate in run time  %6.2f Hz", float(timer.nFrames) / runTime);
  Log::println("frame drop rate         %6.2f %%", frameDropRate * 100.0f);
  Log::println("frame drops           %8lu",       ulong(nFrameDrops));
  Log::println("Run time usage {");
  Log::indent();
  Log::println("Ph0  %6.2f %%  [M] sleep",            sleepTime             / runTime * 100.0f);
  Log::println("Ph1  %6.2f %%  [M] input & ui",       uiTime                / runTime * 100.0f);
  Log::println("Ph2  %6.2f %%  [A] matrix",           matrixTime            / runTime * 100.0f);
  Log::println("     %6.2f %%  [M] loader",           loaderTime            / runTime * 100.0f);
  Log::println("Ph3  %6.2f %%  [A] nirvana",          nirvanaTime           / runTime * 100.0f);
  Log::println("     %6.2f %%  [M] present",          presentTime           / runTime * 100.0f);
  Log::println("     %6.2f %%  [S] + sound",          soundTime             / runTime * 100.0f);
  Log::println("     %6.2f %%  [S]   + effects",      soundEffectsTime      / runTime * 100.0f);
  Log::println("     %6.2f %%  [S]   + music",        soundMusicTime        / runTime * 100.0f);
  Log::println("     %6.2f %%  [M] + render",         renderTime            / runTime * 100.0f);
  Log::println("     %6.2f %%  [M]   + prepare",      renderPrepareTime     / runTime * 100.0f);
  Log::println("     %6.2f %%  [M]   + caelum",       renderCaelumTime      / runTime * 100.0f);
  Log::println("     %6.2f %%  [M]   + terra",        renderTerraTime       / runTime * 100.0f);
  Log::println("     %6.2f %%  [M]   + meshes",       renderMeshesTime      / runTime * 100.0f);
  Log::println("     %6.2f %%  [M]   + misc",         renderMiscTime        / runTime * 100.0f);
  Log::println("     %6.2f %%  [M]   + postprocess",  renderPostprocessTime / runTime * 100.0f);
  Log::println("     %6.2f %%  [M]   + ui",           renderUITime          / runTime * 100.0f);
  Log::println("     %6.2f %%  [M]   + swap",         renderSwapTime        / runTime * 100.0f);
  Log::unindent();
  Log::println("}");
  Log::unindent();
  Log::println("}");

  Log::unindent();
  Log::println("}");
}

void GameStage::init()
{
  Log::println("Initialising GameStage {");
  Log::indent();

  File statePath = appConfig["dir.data"].get(String::EMPTY) + "/state";

  statePath.mkdir();

  autosaveFile  = statePath / "autosave.ozState";
  quicksaveFile = statePath / "quicksave.ozState";

  matrix.init();
  nirvana.init();
  loader.init();
  profile.init();

  saveStream = Stream(0, Endian::LITTLE);

  Log::unindent();
  Log::println("}");
}

void GameStage::destroy()
{
  Log::println("Destroying GameStage {");
  Log::indent();

  profile.destroy();
  loader.destroy();
  nirvana.destroy();
  matrix.destroy();

  stateFile     = "";
  mission       = "";
  autosaveFile  = "";
  quicksaveFile = "";

  Log::unindent();
  Log::println("}");
}

GameStage gameStage;

}
