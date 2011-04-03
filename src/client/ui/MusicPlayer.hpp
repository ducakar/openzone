/*
 *  MusicPlayer.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
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

      int currentTrack;
      const char* title;
      int volume;
      bool isPlaying;

      static void prevTrack( Button* sender );
      static void nextTrack( Button* sender );
      static void playTrack( Button* sender );
      static void stopTrack( Button* sender );
      static void volumeDown( Button* sender );
      static void volumeUp( Button* sender );

    protected:

      virtual void onUpdate();
      virtual void onDraw();

    public:

      MusicPlayer();
      virtual ~MusicPlayer();

  };

}
}
}
