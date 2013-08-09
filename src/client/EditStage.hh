/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file client/EditStage.hh
 */

#pragma once

#include <client/Stage.hh>
#include <client/Proxy.hh>
#include <client/ui/EditFrame.hh>

namespace oz
{
namespace client
{

class EditStage : public Stage
{
  private:

    Thread         auxThread;
    Semaphore      mainSemaphore;
    Semaphore      auxSemaphore;
    volatile bool  isAuxAlive;

  public:

    Proxy*         proxy;

    ui::EditFrame* editFrame;
    File           layoutFile;

  private:

    void read();
    void write() const;

    static void auxMain( void* );
    void auxRun();

  public:

    bool update() override;
    void present( bool isFull ) override;
    void wait( uint micros ) override;

    void load() override;
    void unload() override;

    void init() override;
    void destroy() override;

};

extern EditStage editStage;

}
}
