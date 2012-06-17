/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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

#include "client/Stage.hh"
#include "client/Proxy.hh"

namespace oz
{
namespace client
{

class GameStage : public Stage
{
  public:

    enum State
    {
      GAME,
      MENU
    };

    static String AUTOSAVE_FILE;
    static String QUICKSAVE_FILE;

  private:

    long64          sleepMicros;
    long64          loadingMicros;
    long64          uiMicros;
    long64          loaderMicros;
    long64          soundMicros;
    long64          renderMicros;
    long64          matrixMicros;
    long64          nirvanaMicros;

    Thread          auxThread;
    Semaphore       mainSemaphore;
    Semaphore       auxSemaphore;
    volatile bool   isAuxAlive;

    bool            isLoaded;

  public:

    State  state;
    Proxy* proxy;

    String stateFile;
    String mission;

  private:

    static void auxMain( void* );

    bool read( const char* path );
    void write( const char* path ) const;
    void reload();

    void auxRun();

  public:

    bool update() override;
    void present( bool full ) override;
    void wait( uint micros ) override;

    void load() override;
    void unload() override;

    void init() override;
    void free() override;

};

extern GameStage gameStage;

}
}
