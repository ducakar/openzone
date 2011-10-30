/*
 *  MusicPlayer.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/ui/Frame.hpp"
#include "client/ui/Button.hpp"

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
