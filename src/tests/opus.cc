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

#include <ogg/ogg.h>
#include <opus/opus.h>
#include <vorbis/vorbisfile.h>
#include <AL/alext.h>

using namespace oz;

const int OGG_BUFFER_SIZE = 16 * 1024;
const int FRAME_SIZE      = 960 * 6;

int main()
{
  System::init();
  Window::create("test", 640, 360);

  ALuint source, buffer;
  alGenSources(1, &source);
  alGenBuffers(1, &buffer);

  Stream is = File("/home/davorin/Glasba/Whatever.opus").read();

  ogg_sync_state sync;
  ogg_sync_init(&sync);

  ogg_stream_state stream;
  ogg_stream_init(&stream, 0);

  int          channels = 0;
  OpusDecoder* decoder  = nullptr;
  List<float>  output;

  while (is.available()) {
    int   nBytes = min<int>(OGG_BUFFER_SIZE, is.available());
    char* data   = ogg_sync_buffer(&sync, nBytes);

    is.read(data, nBytes);
    ogg_sync_wrote(&sync, nBytes);

    ogg_page page;
    while (ogg_sync_pageout(&sync, &page) != 0) {
      int serialno = ogg_page_serialno(&page);
      if (stream.serialno != serialno) {
        ogg_stream_reset_serialno(&stream, serialno);
      }

      ogg_stream_pagein(&stream, &page);

      ogg_packet packet;
      while (ogg_stream_packetout(&stream, &packet) == 1) {
        if (packet.b_o_s) {
          channels = packet.packet[9];
          decoder  = opus_decoder_create(48000, channels, nullptr);
        }
        else {
          output.resize(output.length() + FRAME_SIZE);

          int result = opus_decode_float(decoder, packet.packet, int(packet.bytes),
                                         output.end() - FRAME_SIZE, FRAME_SIZE, 0);

          output.resize(output.length() - FRAME_SIZE + max(result, 0) * channels);
        }
      }
    }
  }

  ogg_stream_clear(&stream);
  ogg_sync_clear(&sync);

  alBufferData(buffer, AL_FORMAT_STEREO_FLOAT32, output.begin(), output.length() * 4, 48000);
  alSourcei(source, AL_BUFFER, buffer);

  alSourcePlay(source);

  ALint state;
  do {
    Time::sleep(100);
    alGetSourcei(source, AL_SOURCE_STATE, &state);
  }
  while (state == AL_PLAYING);

  Window::destroy();
  return 0;
}

