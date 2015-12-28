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

#include <SDL2/SDL.h>

using namespace oz;

int main(int argc, char** argv)
{
  System::init();
  SDL_Init(SDL_INIT_VIDEO);
  Window::create("Test", 600, 600, false);

  File file = argc < 2 ? "/usr/share/icons/OpenZone_Ice_Slim/cursors/wait" : argv[1];

  uint texId;
  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);
  GL::textureDataIdenticon(StrongHash()("Davorin"), 600, Vec4(0.20f, 0.30f, 0.25f, 1.00f));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  Cursor cursor(file, Cursor::SYSTEM);

  ALuint buffers[2];
  ALuint source;

  alGenBuffers(2, buffers);
  alGenSources(1, &source);

  AL::Decoder decoder("/home/davorin/Glasba/Whatever.opus", true);
//  AL::Decoder decoder("/home/davorin/Razvoj/openzone/data/oz_base/snd/sfxr/click.wav", false);
//  AL::Decoder decoder("/usr/share/sounds/Oxygen-Sys-App-Error-Critical.ogg", false);

  decoder.decode();
  decoder.load(buffers[0]);
  alSourceQueueBuffers(source, 1, &buffers[0]);

  OZ_AL_CHECK_ERROR();

  if (decoder.decode()) {
    decoder.load(buffers[1]);
    alSourceQueueBuffers(source, 1, &buffers[1]);
  }

  alSourcePlay(source);

  OZ_AL_CHECK_ERROR();

  if (!cursor.isValid()) {
    return EXIT_FAILURE;
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  bool isAlive = true;

  while (isAlive) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        isAlive = false;
      }
    }

    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId);
//    glBindTexture(GL_TEXTURE_2D, cursor.textureId());

    glBegin(GL_QUADS);
//      glTexCoord2i(0, 1); glVertex2d(-1, +1 - 0.02 * cursor.height());
//      glTexCoord2i(1, 1); glVertex2d(-1 + 0.02 * cursor.width(), +1 - 0.02 * cursor.height());
//      glTexCoord2i(1, 0); glVertex2d(-1 + 0.02 * cursor.width(), +1);
//      glTexCoord2i(0, 0); glVertex2d(-1, +1);

      glTexCoord2i(0, 1); glVertex2d(-1, -1);
      glTexCoord2i(1, 1); glVertex2d(+1, -1);
      glTexCoord2i(1, 0); glVertex2d(+1, +1);
      glTexCoord2i(0, 0); glVertex2d(-1, +1);
    glEnd();

    Window::swapBuffers();

    cursor.update(15);

    if (decoder.isValid()) {
      ALint nProcessed;
      alGetSourcei(source, AL_BUFFERS_PROCESSED, &nProcessed);

      if (nProcessed != 0) {
        ALuint buffer;
        alSourceUnqueueBuffers(source, 1, &buffer);

        if (decoder.decode()) {
          decoder.load(buffer);
          alSourceQueueBuffers(source, 1, &buffer);
        }
      }
    }

    Time::sleep(15);
  }

  alDeleteSources(1, &source);
  alDeleteBuffers(2, buffers);

  cursor.destroy();

  glDeleteTextures(1, &texId);

  Window::destroy();
  SDL_Quit();
  return 0;
}
