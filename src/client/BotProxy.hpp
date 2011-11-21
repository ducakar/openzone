/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
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
 * @file client/BotProxy.hpp
 */

#pragma once

#include "client/Proxy.hpp"
#include "client/ui/HudArea.hpp"
#include "client/ui/InfoFrame.hpp"
#include "client/ui/InventoryMenu.hpp"

namespace oz
{
namespace client
{

class BotProxy : public Proxy
{
  private:

    static const float THIRD_PERSON_DIST;
    // leave this much space between obstacle and camera, if camera is brought closer to the eyes
    // because of an obstacle
    static const float THIRD_PERSON_CLIP_DIST;
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
