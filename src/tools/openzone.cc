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

#include <SDL2/SDL.h>
#undef main

#ifdef __native_client__
# include <ppapi/c/ppb_view.h>
# include <ppapi_simple/ps_interface.h>
# include <ppapi_simple/ps_main.h>
#endif

extern "C" void NACL_SetScreenResolution(int width, int height, Uint32 format);

using namespace oz;

static void crashHandler()
{
  SDL_Quit();
}

#if defined(__ANDROID__)
int javaMain(int argc, char** argv)
#elif defined(__native_client__)
int SDL_main(int argc, char** argv)
#else
int main(int argc, char** argv)
#endif
{
  System::init(System::DEFAULT_MASK, &crashHandler);

  Log::printRaw("OpenZone " OZ_VERSION "\n"
                "Copyright © 2002-2019 Davorin Učakar\n"
                "This program comes with ABSOLUTELY NO WARRANTY.\n"
                "This is free software, and you are welcome to redistribute it\n"
                "under certain conditions; See COPYING file for details.\n\n");

#ifdef __native_client__

  Pepper::init();

  const PPB_View* view = PSInterfaceView();

  PSEventSetFilter(PSE_INSTANCE_DIDCHANGEVIEW);

  PP_Rect  rect;
  PSEvent* event = PSEventWaitAcquire();
  view->GetRect(event->as_resource, &rect);

  PSEventRelease(event);

  NACL_SetScreenResolution(rect.size.width, rect.size.height, 0);
  SDL_SetMainReady();

#endif

  int exitCode = client::client.init(argc, argv);

  if (exitCode == EXIT_SUCCESS) {
    exitCode = client::client.main();
  }

  client::client.shutdown();

  return exitCode;
}

#ifdef __native_client__
PSMainFunc_t PSUserMainGet() { return SDL_main; }
#endif
OZ_JAVA_ENTRY_POINT(Java_com_github_ducakar_openzone_SDLActivity_nativeInit)
