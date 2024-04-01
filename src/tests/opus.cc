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

#include <ozCore/ozCore.hh>
#include <ozEngine/ozEngine.hh>

#include <ogg/ogg.h>
#include <opus/opus.h>

using namespace oz;

namespace
{

constexpr int FRAME_SIZE = 120 * 48;
constexpr int OGG_BUFFER = 4 * 1024;

List<float>   samples;
int           nSamples = 0;

ALuint        buffers[2];
ALuint        source;
Semaphore     decodeMainSemaphore;
Semaphore     decodeThreadSemaphore;
Thread        decodeThread;
Atomic<bool>  isDecoderAlive = {true};

void* decoderMain(void*)
{
  Stream is(0);
  File("/home/davorin/Glasba/Whatever1.opus").read(&is);

  ogg_sync_state sync;
  ogg_sync_init(&sync);

  ogg_stream_state stream;
  ogg_stream_init(&stream, 0);

  int          nChannels = 0;
  OpusDecoder* decoder   = nullptr;

  while (isDecoderAlive.load<ACQUIRE>() && is.available()) {
    int   nBytes         = min<int>(OGG_BUFFER, is.available());
    char* data           = ogg_sync_buffer(&sync, nBytes);
    int   nHeaderPackets = 0;

    is.read(data, nBytes);
    ogg_sync_wrote(&sync, nBytes);

    ogg_page page;
    while (isDecoderAlive.load<RELAXED>() && ogg_sync_pageout(&sync, &page) != 0) {
      int serialno = ogg_page_serialno(&page);
      if (stream.serialno != serialno) {
        ogg_stream_reset_serialno(&stream, serialno);
      }

      ogg_stream_pagein(&stream, &page);

      ogg_packet packet;
      while (isDecoderAlive.load<RELAXED>() && ogg_stream_packetout(&stream, &packet) == 1) {
        if (nHeaderPackets != 0) {
          --nHeaderPackets;
        }
        else if (packet.b_o_s) {
          nChannels = opus_packet_get_nb_channels(packet.packet);
          decoder   = opus_decoder_create(48000, nChannels, nullptr);

          samples.resize(2 * FRAME_SIZE * nChannels);
          nHeaderPackets = 1;
        }
        else if (decoder != nullptr) {
          int result = opus_decode_float(decoder, packet.packet, int(packet.bytes),
                                         samples.begin() + nSamples, FRAME_SIZE, 0);

          if (result <= 0) {
            if (nSamples != 0) {
              decodeMainSemaphore.post();
              decodeThreadSemaphore.wait();

              nSamples = 0;
            }

            goto finishedDecoding;
          }
          else {
            nSamples += result * nChannels;

            if (nSamples + FRAME_SIZE * nChannels > samples.size()) {
              decodeMainSemaphore.post();
              decodeThreadSemaphore.wait();

              nSamples = 0;
            }
          }
        }
      }
    }
  }
finishedDecoding:

  nSamples = 0;

  ogg_stream_clear(&stream);
  ogg_sync_clear(&sync);

  while (isDecoderAlive.load<RELAXED>()) {
    decodeMainSemaphore.post();
    decodeThreadSemaphore.wait();
  }

  return nullptr;
}

class MainStage : public Application::Stage
{
public:

  void load() override;
  void unload() override;
  void update() override;
  void present(bool isEnoughTime) override;

};

void MainStage::load()
{
  alGenBuffers(2, buffers);
  alGenSources(1, &source);

  decodeThread = Thread("decode", decoderMain);

  decodeMainSemaphore.wait();

  if (nSamples <= 0) {
    Application::setStage(nullptr);
  }
  else {
    alBufferData(buffers[0], AL::FORMAT_STEREO_FLOAT32, samples.begin(), nSamples * 4, 48000);

    decodeThreadSemaphore.post();
    decodeMainSemaphore.wait();

    if (nSamples <= 0) {
      Application::setStage(nullptr);
    }
    else {
      alBufferData(buffers[1], AL::FORMAT_STEREO_FLOAT32, samples.begin(), nSamples * 4, 48000);
      alSourceQueueBuffers(source, 2, buffers);
      alSourcePlay(source);

      decodeThreadSemaphore.post();
    }
  }
}

void MainStage::unload()
{
  isDecoderAlive.store<RELAXED>(false);

  decodeThreadSemaphore.post();
  decodeThread.join();

  alDeleteSources(1, &source);
  alDeleteBuffers(2, buffers);
}

void MainStage::update()
{
  ALint nProcessed = 0;
  alGetSourcei(source, AL_BUFFERS_PROCESSED, &nProcessed);

  if (nProcessed != 0) {
    ALuint buffer = 0;
    alSourceUnqueueBuffers(source, 1, &buffer);

    decodeMainSemaphore.wait();

    if (nSamples <= 0) {
      Application::setStage(nullptr);
    }
    else {
      Log() << nSamples;
      alBufferData(buffer, AL::FORMAT_STEREO_FLOAT32, samples.begin(), nSamples * 4, 48000);
      alSourceQueueBuffers(source, 1, &buffer);

      OZ_AL_CHECK_ERROR();

      decodeThreadSemaphore.post();
    }
  }
}

void MainStage::present(bool)
{
//  if (isEnoughTime) {
//    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
//    glClear(GL_COLOR_BUFFER_BIT);

//    Window::swapBuffers();
//  }
}

MainStage mainStage;

}

int main()
{
  System::init();

  Application::defaults.name         = "scratch";
  Application::defaults.window.title = "Scratch";
  Application::defaults.timing.fps   = 200;

  Application::run(&mainStage);
  return 0;
}
