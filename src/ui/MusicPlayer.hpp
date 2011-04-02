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

#include "ui/Frame.hpp"
#include "ui/Button.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  class MusicPlayer : public Frame
  {
    private:

      int currentSong;
      const char* title;
      int volume;
      bool isPlaying;

      static void prevSong( Button* sender );
      static void nextSong( Button* sender );
      static void playSong( Button* sender );
      static void stopSong( Button* sender );
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
