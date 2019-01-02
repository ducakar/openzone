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

/**
 * @file client/Client.hh
 *
 * Game initialisation and main loop.
 */

#pragma once

#include <client/Stage.hh>

#define OZ_ANDROID_ROOT "/data/data/com.github.ducakar/openzone"

namespace oz::client
{

class Client
{
private:

  static constexpr int INIT_CONFIG     = 0x0001;
  static constexpr int INIT_WINDOW     = 0x0008;
  static constexpr int INIT_INPUT      = 0x0010;
  static constexpr int INIT_NETWORK    = 0x0020;
  static constexpr int INIT_LINGUA     = 0x0040;
  static constexpr int INIT_LIBRARY    = 0x0080;
  static constexpr int INIT_SDL_TTF    = 0x0100;
  static constexpr int INIT_CONTEXT    = 0x0200;
  static constexpr int INIT_RENDER     = 0x0400;
  static constexpr int INIT_AUDIO      = 0x0800;
  static constexpr int INIT_STAGE_INIT = 0x1000;
  static constexpr int INIT_STAGE_LOAD = 0x2000;
  static constexpr int INIT_MAIN_LOOP  = 0x4000;

private:

  Stage*   stage;
  int      initFlags;

  int      windowWidth;
  int      windowHeight;
  int      screenWidth;
  int      screenHeight;

  Duration benchmarkDuration;
  bool     isBenchmark;

private:

  void printUsage();

public:

  int main();

  int init(int argc, char** argv);
  void shutdown();

};

extern Client client;

}
