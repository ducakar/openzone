/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/BotProxy.hh
 */

#pragma once

#include "client/Proxy.hh"
#include "client/ui/HudArea.hh"
#include "client/ui/InfoFrame.hh"
#include "client/ui/InventoryMenu.hh"

namespace oz
{
namespace client
{

class BotProxy : public Proxy
{
  private:

    static const float EXTERNAL_CAM_DIST;
    // leave this much space between obstacle and camera, if camera is brought closer to the eyes
    // because of an obstacle
    static const float EXTERNAL_CAM_CLIP_DIST;
    static const float SHOULDER_CAM_RIGHT;
    static const float SHOULDER_CAM_UP;
    static const float VEHICLE_CAM_UP_FACTOR;
    static const float BOB_SUPPRESSION_COEF;

    ui::HudArea*       hud;
    ui::InfoFrame*     infoFrame;
    ui::InventoryMenu* inventory;
    ui::InventoryMenu* container;

    float bobPhi;
    float bobTheta;
    float bobBias;

    bool  isExternal;
    bool  isFreelook;

  public:

    BotProxy();

    virtual void begin();
    virtual void end();

    virtual void update();
    virtual void prepare();

    virtual void reset();

    virtual void read( InputStream* istream );
    virtual void write( BufferStream* ostream ) const;

    void init();

};

}
}
