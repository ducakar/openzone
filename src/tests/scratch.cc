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

#include <ozCore/ozCore.hh>
#include <ozEngine/ozEngine.hh>

using namespace oz;

int main()
{
  System::init();
  Window::create("Drek", 400, 300, false);

  uint buffers[2], source;
  alGenBuffers(2, buffers);
  alGenSources(1, &source);
  AL::bufferDataFromFile(buffers[0], "/usr/share/sounds/Kopete_Received.ogg");
  AL::bufferDataFromFile(buffers[1], "/usr/share/sounds/Oxygen-Sys-App-Positive.ogg");

  alSourcei(source, AL_BUFFER, buffers[0]);
  alSourcePlay(source);
  alSourceStop(source);
  alSourcei(source, AL_BUFFER, buffers[1]);
  alSourcePlay(source);

  bool isAlive = true;
  while (isAlive) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      isAlive = Window::processEvent(&event);
      Input::processEvent(&event);
    }
    Window::swapBuffers();
    Time::sleep(10);

    Log() << Input::mouse.x << ", " << Input::mouse.y;
  }

  alDeleteSources(1, &source);
  alDeleteBuffers(2, buffers);

  Window::destroy();
  return 0;
}
