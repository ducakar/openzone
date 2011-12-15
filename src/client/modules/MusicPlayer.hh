/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
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
 */

/**
 * @file client/modules/MusicPlayer.hh
 */

#pragma once

#include "client/ui/Frame.hh"
#include "client/ui/Button.hh"

namespace oz
{
namespace client
{
namespace ui
{

class MusicPlayer : public Frame
{
  private:

    Label title;
    Label trackLabel;
    Label volumeLabel;
    int   currentTrack;
    int   volume;
    bool  isPlaying;
    bool  isVisible;

    static void prevTrack( Button* sender );
    static void nextTrack( Button* sender );
    static void playTrack( Button* sender );
    static void stopTrack( Button* sender );
    static void volumeDown( Button* sender );
    static void volumeUp( Button* sender );

  protected:

    virtual void onUpdate();
    virtual bool onMouseEvent();
    virtual void onDraw();

  public:

    MusicPlayer();
    virtual ~MusicPlayer();

};

}
}
}
