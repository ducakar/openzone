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
 * @file client/openzone.cc
 */

#include <client/Client.hh>

#include <SDL.h>
#undef main

using namespace oz;

static void crashHandler()
{
  SDL_Quit();
}

#if defined(__ANDROID__)
int javaMain(int argc, char** argv)
#elif defined(__native_client__)
int naclMain(int argc, char** argv)
#elif defined(_WIN32)
int SDL_main(int argc, char** argv)
#else
int main(int argc, char** argv)
#endif
{
  System::init(System::DEFAULT_MASK, &crashHandler);

  Log::printRaw("OpenZone " OZ_VERSION "\n"
                "Copyright © 2002-2014 Davorin Učakar\n"
                "This program comes with ABSOLUTELY NO WARRANTY.\n"
                "This is free software, and you are welcome to redistribute it\n"
                "under certain conditions; See COPYING file for details.\n\n");

  int exitCode = client::client.init(argc, argv);

  if (exitCode == EXIT_SUCCESS) {
    exitCode = client::client.main();
  }

  client::client.shutdown();

  if (Alloc::count != 0) {
    Log::verboseMode = true;
    bool isOutput = Log::printMemoryLeaks();
    Log::verboseMode = false;

    if (isOutput) {
      Log::println("There are some memory leaks. See '%s' for details.", Log::file().c());
    }
  }

  return exitCode;
}

OZ_JAVA_ENTRY_POINT(Java_com_github_ducakar_openzone_SDLActivity_nativeInit)
OZ_NACL_ENTRY_POINT()
