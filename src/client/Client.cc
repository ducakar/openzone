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

#include <client/Client.hh>

#include <client/Network.hh>
#include <client/Input.hh>
#include <client/Context.hh>
#include <client/Sound.hh>
#include <client/Render.hh>
#include <client/Loader.hh>
#include <client/BuildInfo.hh>
#include <client/MenuStage.hh>
#include <client/GameStage.hh>
#include <client/EditStage.hh>
#include <client/ui/UI.hh>

#include <gamemode_client.h>

#include <SDL2/SDL.h>
#include <unistd.h>
#ifdef __native_client__
# include <ppapi_simple/ps.h>
# include <ppapi_simple/ps_event.h>
# include <ppapi_simple/ps_instance.h>
# include <ppapi_simple/ps_interface.h>
# include <sys/mount.h>
#endif

#undef main

namespace oz::client
{

#ifdef __native_client__
static const PPB_View*            ppbView            = nullptr;
static const PPB_InputEvent*      ppbInputEvent      = nullptr;
static const PPB_MouseInputEvent* ppbMouseInputEvent = nullptr;
#endif

void Client::printUsage()
{
  Log::printRaw(
    "Usage: openzone [-v] [-l | -i <mission>] [-t <num>] [-L <lang>] [-p <prefix>]\n"
    "  -l            Skip main menu and load the last autosaved game.\n"
    "  -i <mission>  Skip main menu and start mission <mission>.\n"
    "  -e <layout>   Edit world <layout> file. Create a new one if non-existent.\n"
    "  -t <num>      Exit after <num> seconds (can be a floating-point number) and\n"
    "                use 42 as the random seed. Useful for benchmarking.\n"
    "  -L <lang>     Use language <lang>. Should match a subdirectory name in\n"
    "                'lingua/' directory inside game data archives.\n"
    "                Defaults to 'en'.\n"
    "  -p <prefix>   Set global data directory to '<prefix>/share/openzone'.\n"
    "                Defaults to '%s'.\n"
    "  -v            Print verbose log messages to terminal.\n\n", OZ_PREFIX);
}

int Client::main()
{
#ifdef __native_client__
  List<PSEvent*> eventQueue(8);
  PSEvent*       psEvent;
#endif

  SDL_Event event;

  bool            isAlive   = true;
  bool            isActive  = true;
  // Time spent on the current frame so far.
  Duration        timeSpent = Duration::ZERO;
  Instant<STEADY> timeZero  = Instant<STEADY>::now();
  // Time at the end of the last frame.
  Instant<STEADY> timeLast  = timeZero;

  initFlags |= INIT_MAIN_LOOP;

  Log::println("Main loop {");
  Log::indent();

  // THE MAGNIFICENT MAIN LOOP
  do {
    // read input & events
    input.prepare();

#ifdef __native_client__

    eventQueue.clear();

    // SDL implementation for NaCl is still incomplete and it doesn't handle certain events, so we
    // have to do them manually.
    while ((psEvent = PSEventTryAcquire()) != nullptr) {
      switch (psEvent->type) {
        case PSE_INSTANCE_HANDLEINPUT: {
          if (ppbInputEvent->GetType(psEvent->as_resource) == PP_INPUTEVENT_TYPE_MOUSEMOVE) {
            PP_Point point = ppbMouseInputEvent->GetMovement(psEvent->as_resource);

            input.mouseX += point.x;
            input.mouseY -= point.y;
          }
          else {
            eventQueue.add(psEvent);
          }
          break;
        }
        case PSE_INSTANCE_DIDCHANGEVIEW: {
          PP_Rect rect;
          ppbView->GetRect(psEvent->as_resource, &rect);

          SDL_Event event;
          event.type = SDL_WINDOWEVENT;
          event.window.event = SDL_WINDOWEVENT_RESIZED;
          event.window.data1 = rect.size.width;
          event.window.data2 = rect.size.height;

          SDL_PushEvent(&event);
          break;
        }
        default: {
          eventQueue.add(psEvent);
          break;
        }
      }

      PSEventRelease(psEvent);
    }

    // Repost relevant events for SDL to process them.
    for (PSEvent* event : eventQueue) {
      PSEventPostResource(event->type, event->as_resource);
    }

    // Make sure it doesn't grow too much.
    if (eventQueue.length() > 8) {
      eventQueue.resize(8, true);
    }

#endif

    SDL_PumpEvents();

    while (SDL_PollEvent(&event) != 0) {
      isAlive = Window::processEvent(&event);

      switch (event.type) {
        case SDL_MOUSEMOTION:
        case SDL_MOUSEWHEEL:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_KEYUP: {
          input.readEvent(&event);
          break;
        }
        case SDL_KEYDOWN: {
          const SDL_Keysym& keysym = event.key.keysym;

          if (keysym.sym == SDLK_F9) {
            if (keysym.mod & KMOD_CTRL) {
              ui::ui.isVisible = !ui::ui.isVisible;
            }
#ifndef __native_client__
            else {
              loader.makeScreenshot();
            }
#endif
          }
          else if (keysym.sym == SDLK_F11) {
            if (keysym.mod & KMOD_CTRL) {
              Window::setGrab(!Window::hasGrab());
            }
            else if (Window::mode() == Window::WINDOWED) {
              Window::resize(0, 0, Window::DESKTOP);
            }
            else {
              Window::resize(windowWidth, windowHeight, Window::WINDOWED);
            }
            input.reset();
          }
#ifndef __native_client__
          else if (keysym.sym == SDLK_F12) {
            if (keysym.mod & KMOD_CTRL) {
              isAlive = false;
            }
            else {
              Window::minimise();
              input.reset();
            }
          }
#endif

          input.readEvent(&event);
          break;
        }
        case SDL_WINDOWEVENT: {
          switch (event.window.event) {
            case SDL_WINDOWEVENT_FOCUS_GAINED: {
              input.reset();
              break;
            }
            case SDL_WINDOWEVENT_FOCUS_LOST: {
              input.reset();
              break;
            }
            case SDL_WINDOWEVENT_SIZE_CHANGED: {
              input.reset();
              break;
            }
#ifndef __native_client__
            case SDL_WINDOWEVENT_RESTORED: {
              input.reset();
              sound.resume();

              isActive = true;
              break;
            }
            case SDL_WINDOWEVENT_MINIMIZED: {
              sound.suspend();
              input.reset();

              isActive = false;
              break;
            }
#endif
          }
          break;
        }
        default: {
          break;
        }
      }
    }

#ifdef __native_client__

    input.keys[SDLK_ESCAPE]    = false;
    input.oldKeys[SDLK_ESCAPE] = false;

#endif

    // Waste time when iconified.
    if (!isActive) {
      Thread::sleepFor(timer.realTickDuration);

      timeSpent = Instant<STEADY>::now() - timeLast;
      timeLast += timeSpent;

      continue;
    }

    input.update();

    timer.tick();

    isAlive &= !isBenchmark || timer.duration < benchmarkDuration;
    isAlive &= stage->update();

    if (Stage::nextStage != nullptr) {
      stage->unload();

      stage = Stage::nextStage;
      Stage::nextStage = nullptr;

      input.prepare();
      input.update();

      stage->load();

      timeLast = Instant<STEADY>::now();
      continue;
    }

    timeSpent = Instant<STEADY>::now() - timeLast;

    // Skip rendering graphics, only play sounds if there's not enough time left.
    if (timeSpent >= timer.realTickDuration && timer.frameDuration < 100_ms) {
      stage->present(false);
    }
    else {
      stage->present(true);
      timer.frame();

      // If there's still some time left, sleep.
      timeSpent = Instant<STEADY>::now() - timeLast;

      if (timeSpent < timer.realTickDuration) {
        stage->wait(timer.realTickDuration - timeSpent);
        timeSpent = timer.realTickDuration;
      }
    }

    if (timeSpent > 100_ms) {
      timer.drop(timeSpent - timer.realTickDuration);
      timeLast += timeSpent - timer.realTickDuration;
    }
    timeLast += timer.realTickDuration;
  }
  while (isAlive);

  Log::unindent();
  Log::println("}");

  return EXIT_SUCCESS;
}

int Client::init(int argc, char** argv)
{
#ifdef __native_client__
  Pepper::post("init");
#endif

  initFlags         = 0;
  isBenchmark       = false;
  benchmarkDuration = Duration::ZERO;

  File   prefixDir  = OZ_PREFIX;
  String language   = "";
  String mission    = "";
  String layoutFile = "";
  bool   doAutoload = false;

  // Standalone. Executable is ./bin/<platform>/openzone.
  if (prefixDir.isEmpty()) {
    prefixDir = File::EXECUTABLE.directory() / "../..";
  }

  optind = 1;
  int opt;
  while ((opt = getopt(argc, argv, "li:e:t:L:p:vhH?")) != -1) {
    switch (opt) {
      case 'l': {
        doAutoload = true;
        break;
      }
      case 'i': {
        mission = optarg;
        break;
      }
      case 'e': {
        layoutFile = optarg;
        break;
      }
      case 't': {
        const char* end;
        benchmarkDuration = String::parseDouble(optarg, &end) * 1_s;

        if (end == optarg) {
          printUsage();
          return EXIT_FAILURE;
        }

        isBenchmark = true;
        break;
      }
      case 'L': {
        language = optarg;
        break;
      }
      case 'p': {
        prefixDir = optarg;
        break;
      }
      case 'v': {
        Log::showVerbose = true;
        break;
      }
      default: {
        printUsage();
        return EXIT_FAILURE;
      }
    }
  }

#ifdef __native_client__

  // Wait until web page updates game data and sends the language code.
  for (String message = Pepper::pop(); ; message = Pepper::pop()) {
    if (message.isEmpty()) {
      Time::sleep(10);
    }
    else {
      language = message;
      break;
    }
  }

  umount("");
  mount("", "/", "html5fs", 0, "type=TEMPORARY");
  mount("", "/data", "memfs", 0, "");

#endif

  File::init();

#ifdef __ANDROID__

  File configDir   = OZ_ANDROID_ROOT "/config";
  File dataDir     = OZ_ANDROID_ROOT "/data";
  File picturesDir = "";
  File musicDir    = "";

#else

  File::CONFIG.mkdir();
  File::DATA.mkdir();

  File configDir   = File::CONFIG / "openzone";
  File dataDir     = File::DATA / "openzone";
  File musicDir    = File::MUSIC.isEmpty() ? File::MUSIC : File::MUSIC / "OpenZone";
  File picturesDir = File::PICTURES.isEmpty() ? File::PICTURES : File::PICTURES / "OpenZone";

#endif

  configDir.mkdir();
  dataDir.mkdir();

  if (Log::init(configDir / "client.log", true)) {
    Log::println("Log file '%s'", Log::file().c());
  }

  Log::println("OpenZone " OZ_VERSION " started on %s", Time::local().toString().c());

  Log::println("Build details {");
  Log::indent();
  Log::println("Date:            %s", BuildInfo::TIME);
  Log::println("Host:            %s", BuildInfo::HOST);
  Log::println("Host arch:       %s", BuildInfo::HOST_ARCH);
  Log::println("Target arch:     %s", BuildInfo::TARGET_ARCH);
  Log::println("Build type:      %s", BuildInfo::BUILD_TYPE);
  Log::println("Compiler:        %s", BuildInfo::COMPILER);
  Log::println("Compiler flags:  %s", BuildInfo::CXX_FLAGS);
  Log::println("Configuration:   %s", BuildInfo::CONFIG);
  Log::unindent();
  Log::println("}");

  dataDir.mountAt("/");

  // Clean up after previous versions. Be evil. Delete everything.
  File screenshotDir = configDir / "screenshots";
  File savesDir      = configDir / "saves";

  for (const File& file : screenshotDir.list()) {
    file.remove();
  }
  screenshotDir.remove();

  for (const File& file : savesDir.list()) {
    file.remove();
  }
  savesDir.remove();

  (configDir / "client.rc").remove();

  // Load configuration.
  File configFile = configDir / "client.json";

  if (appConfig.load(configFile) && String::equals(appConfig["_version"].get(""), OZ_VERSION)) {
    Log::printEnd("Configuration read from '%s'", configFile.c());
    initFlags |= INIT_CONFIG;
  }
  else {
    Log::println("Invalid configuration file version, default settings used.");

    appConfig = Json::OBJECT;
    appConfig.add("_version", OZ_VERSION).get(String::EMPTY);
  }

  appConfig.add("dir.config", configDir).get(String::EMPTY);
  appConfig.add("dir.data", dataDir).get(String::EMPTY);
  appConfig.include("dir.pictures", picturesDir).get(String::EMPTY);
  appConfig.include("dir.music", musicDir).get(String::EMPTY);
  appConfig.include("dir.prefix", prefixDir).get(String::EMPTY);

  Log::print("Enabling game mode ...");
  if (gamemode_request_start() == 0) {
    Log::printEnd(" OK");
  }
  else {
    Log::printEnd(" Failed");
  }

  windowWidth  = appConfig.include("window.windowWidth",  1280).get(0);
  windowHeight = appConfig.include("window.windowHeight", 720 ).get(0);
  screenWidth  = appConfig.include("window.screenWidth",  0   ).get(0);
  screenHeight = appConfig.include("window.screenHeight", 0   ).get(0);

  bool fullscreen = appConfig.include("window.fullscreen", true).get(false);

  if (!Window::create("OpenZone " OZ_VERSION,
                      fullscreen ? screenWidth : windowWidth,
                      fullscreen ? screenHeight : windowHeight,
                      fullscreen ? Window::DESKTOP : Window::WINDOWED))
  {
    OZ_ERROR("Window creation failed");
  }

  initFlags |= INIT_WINDOW;

  input.init();
  initFlags |= INIT_INPUT;

  network.init();
  initFlags |= INIT_NETWORK;

#ifdef __native_client__

  // Copy game packages to memfs.
  for (const File& package : File("/cache").list("zip")) {
    package.copyTo("/data/openzone/" + package.name());
  }

#endif

  Log::println("Content search path {");
  Log::indent();

#if !defined(__ANDROID__) && !defined(__native_client__)

  File globalDataDir = appConfig["dir.prefix"].get(String::EMPTY) + "/share/openzone";
  File userMusicDir  = appConfig["dir.music"].get(File::MUSIC);

  if (userMusicDir.mountAt("/userMusic", true)) {
    Log::println("%s [mounted on /userMusic]", userMusicDir.c());
  }

#endif

  if (dataDir.mountAt(nullptr, true)) {
    Log::println("%s", dataDir.c());

    for (const File& file : dataDir.list("zip")) {
      if (!file.mountAt(nullptr)) {
        OZ_ERROR("Failed to mount '%s' on / in PhysicsFS: %s",
                 file.c(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
      }
      Log::println("%s", file.c());
    }
  }

#if !defined(__ANDROID__) && !defined(__native_client__)

  if (globalDataDir.mountAt(nullptr, true)) {
    Log::println("%s", globalDataDir.c());

    for (const File& file : globalDataDir.list("zip")) {
      if (!file.mountAt(nullptr)) {
        OZ_ERROR("Failed to mount '%s' on / in PhysicsFS: %s",
                 file.c(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
      }
      Log::println("%s", file.c());
    }
  }

#endif

  Log::unindent();
  Log::println("}");

  appConfig.include("lingua", "AUTO").get(String::EMPTY);

  if (language.isEmpty()) {
    language = appConfig["lingua"].get("AUTO");
  }
  if (language == "AUTO") {
    language = Lingua::detectLanguage("en");
  }

  Log::print("Setting language '%s' ...", language.c());
  if (lingua.init(language)) {
    Log::printEnd(" OK");

    initFlags |= INIT_LINGUA;
  }
  else {
    Log::printEnd(" Failed");
  }

  appConfig.include("seed", "TIME");

  int seed;

  if (appConfig["seed"].type() == Json::STRING) {
    if (appConfig["seed"].get(String::EMPTY) != "TIME") {
      OZ_ERROR("Configuration variable 'sees' must be either \"TIME\" or an integer");
    }

    seed = int(Time::epoch());
  }
  else {
    seed = appConfig["seed"].get(42);
  }

  if (isBenchmark) {
    seed = 42;
  }

  Math::seed(seed);
  Lua::randomSeed = seed;

  Log::println("Random generator seed set to: %u", seed);

  sound.initLibs();

  initFlags |= INIT_LIBRARY;
  liber.init(appConfig["dir.music"].get(""));

  Font::init();
  initFlags |= INIT_SDL_TTF;

  initFlags |= INIT_CONTEXT;
  context.init();

  initFlags |= INIT_RENDER;
  render.init();

  initFlags |= INIT_AUDIO;
  sound.init();

  initFlags |= INIT_STAGE_INIT;
  menuStage.init();
  gameStage.init();

#ifdef __native_client__
  Pepper::post("none");

  ppbView            = PSInterfaceView();
  ppbInputEvent      = PSInterfaceInputEvent();
  ppbMouseInputEvent = static_cast<const PPB_MouseInputEvent*>(
                         PSGetInterface(PPB_MOUSE_INPUT_EVENT_INTERFACE));
#endif

  Stage::nextStage = nullptr;

  if (!layoutFile.isEmpty()) {
    editStage.layoutFile = layoutFile;

    stage = &editStage;
  }
  else if (!mission.isEmpty()) {
    gameStage.mission = mission;

    stage = &gameStage;
  }
  else if (doAutoload) {
    gameStage.stateFile = gameStage.autosaveFile;
    stage = &gameStage;
  }
  else {
    stage = &menuStage;
  }

  stage->load();

#ifndef __native_client__
  Window::setGrab(true);
#endif
  input.reset();

  return EXIT_SUCCESS;
}

void Client::shutdown()
{
  if (stage != nullptr) {
    stage->unload();
  }

  if (initFlags & INIT_STAGE_INIT) {
    gameStage.destroy();
    menuStage.destroy();
  }
  if (initFlags & INIT_AUDIO) {
    sound.destroy();
  }
  if (initFlags & INIT_RENDER) {
    render.destroy();
  }
  if (initFlags & INIT_CONTEXT) {
    context.destroy();
  }
  if (initFlags & INIT_SDL_TTF) {
    Font::destroy();
  }
  if (initFlags & INIT_LIBRARY) {
    liber.destroy();
  }
  if (initFlags & INIT_LINGUA) {
    lingua.destroy();
  }
  if (initFlags & INIT_NETWORK) {
    network.destroy();
  }
  if (initFlags & INIT_INPUT) {
    input.destroy();
  }
  if (initFlags & INIT_WINDOW) {
    Window::destroy();
  }
  if (initFlags & INIT_MAIN_LOOP) {
    File configFile = appConfig["dir.config"].get(File::CONFIG) + "/client.json";

    if (!(initFlags & INIT_CONFIG)) {
      appConfig.exclude("dir.config");
      appConfig.exclude("dir.data");

      Log::print("Writing configuration to '%s' ...", configFile.c());
      appConfig.save(configFile, CONFIG_FORMAT);
      Log::printEnd(" OK");
    }
  }

  appConfig.clear(initFlags & INIT_CONFIG);

  File::destroy();

  Log::printProfilerStatistics();
  Profiler::clear();

  Log::printMemorySummary();

  if (initFlags) {
    Log::println("OpenZone " OZ_VERSION " finished on %s", Time::local().toString().c());
  }

#ifdef __native_client__
  Pepper::post("quit");
#endif
}

Client client;

}
