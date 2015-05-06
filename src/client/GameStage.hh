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
 * @file client/GameStage.hh
 */

#pragma once

#include <client/Stage.hh>
#include <client/Proxy.hh>

namespace oz
{
namespace client
{

class GameStage : public Stage
{
private:

  // 2.5 min.
  static const uint AUTOSAVE_INTERVAL;

  ulong64       startTicks;
  long64        sleepMicros;
  long64        loadingMicros;
  long64        uiMicros;
  long64        loaderMicros;
  long64        presentMicros;
  long64        matrixMicros;
  long64        nirvanaMicros;

  uint          autosaveTicks;

  Buffer        saveBuffer;
  OutputStream  saveStream;
  File          saveFile;
  Thread        saveThread;

  Thread        auxThread;
  Semaphore     mainSemaphore;
  Semaphore     auxSemaphore;
  volatile bool isAuxAlive;

public:

  Proxy*        proxy;

  File          autosaveFile;
  File          quicksaveFile;
  File          stateFile;
  String        mission;

private:

  static void saveMain(void*);

  void read();
  void write();

  static void auxMain(void*);
  void auxRun();

public:

  bool update() override;
  void present(bool isFull) override;
  void wait(uint micros) override;

  void load() override;
  void unload() override;

  void init() override;
  void destroy() override;

};

extern GameStage gameStage;

}
}
