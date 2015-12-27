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
  Window::create("scratch", 640, 360);

  ALuint buffers[2];
  ALuint source;

  alGenBuffers(2, buffers);
  alGenSources(1, &source);

  AL::Decoder decoder("/usr/share/sounds/uget/notification.wav", true);

  decoder.decode();
  decoder.load(buffers[0]);
  alSourceQueueBuffers(source, 1, &buffers[0]);

  if (decoder.decode()) {
    decoder.load(buffers[1]);
    alSourceQueueBuffers(source, 1, &buffers[1]);
  }

  alSourcePlay(source);

  do {
    Time::sleep(10);

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
  while (decoder.isValid());

  ALint state;
  do {
    Time::sleep(100);
    alGetSourcei(source, AL_SOURCE_STATE, &state);
  }
  while (state == AL_PLAYING);

  alDeleteSources(1, &source);
  alDeleteBuffers(2, buffers);

  Window::destroy();
  return 0;
}
