/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

#include <client/ui/MusicPlayer.hh>

#include <client/Camera.hh>
#include <client/Sound.hh>
#include <client/ui/Mouse.hh>
#include <client/ui/Style.hh>

namespace oz
{
namespace client
{
namespace ui
{

void MusicPlayer::prevTrack(Button* sender)
{
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>(sender->parent);
  int nTracks = liber.musicTracks.length();

  if (nTracks != 0) {
    musicPlayer->currentTrack = (musicPlayer->currentTrack + nTracks - 1) % nTracks;

    sound.playMusic(musicPlayer->currentTrack);

    musicPlayer->trackTitle.setText("%s", liber.musicTracks[musicPlayer->currentTrack].name.c());
    musicPlayer->trackLabel.setText("%d", musicPlayer->currentTrack + 1);
    musicPlayer->isPlaying = true;
  }
}

void MusicPlayer::nextTrack(Button* sender)
{
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>(sender->parent);
  int nTracks = liber.musicTracks.length();

  if (nTracks != 0) {
    musicPlayer->currentTrack = (musicPlayer->currentTrack + 1) % nTracks;

    sound.playMusic(musicPlayer->currentTrack);

    musicPlayer->trackTitle.setText("%s", liber.musicTracks[musicPlayer->currentTrack].name.c());
    musicPlayer->trackLabel.setText("%d", musicPlayer->currentTrack + 1);
    musicPlayer->isPlaying = true;
  }
}

void MusicPlayer::playTrack(Button* sender)
{
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>(sender->parent);
  int nTracks = liber.musicTracks.length();

  if (nTracks != 0) {
    sound.playMusic(musicPlayer->currentTrack);

    musicPlayer->trackTitle.setText("%s", liber.musicTracks[musicPlayer->currentTrack].name.c());
    musicPlayer->trackLabel.setText("%d", musicPlayer->currentTrack + 1);
    musicPlayer->isPlaying = true;
  }
}

void MusicPlayer::stopTrack(Button* sender)
{
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>(sender->parent);

  sound.stopMusic();

  musicPlayer->trackTitle.setText("");
  musicPlayer->isPlaying = false;
}

void MusicPlayer::volumeDown(Button* sender)
{
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>(sender->parent);

  musicPlayer->volume = max(musicPlayer->volume - 1, 0);
  musicPlayer->volumeLabel.setText("%d", musicPlayer->volume);

  sound.setMusicVolume(float(musicPlayer->volume) / 10.0f);
}

void MusicPlayer::volumeUp(Button* sender)
{
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>(sender->parent);

  musicPlayer->volume = min(musicPlayer->volume + 1, 10);
  musicPlayer->volumeLabel.setText("%d", musicPlayer->volume);

  sound.setMusicVolume(float(musicPlayer->volume) / 10.0f);
}

void MusicPlayer::onUpdate()
{
  const Bot* bot = camera.botObj;

  if (camera.state == Camera::UNIT && (bot == nullptr || (bot->state & Bot::DEAD_BIT) ||
                                       !bot->hasAttribute(ObjectClass::MUSIC_PLAYER_BIT)))
  {
    if (isPlaying) {
      isPlaying = false;
      trackTitle.setText("");
      sound.stopMusic();
    }

    show(false);
    return;
  }
  else if (mouse.isVisible) {
    show(true);
  }

  int soundTrack = sound.getCurrentTrack();
  if (isPlaying && soundTrack != currentTrack) {
    if (soundTrack == -1) {
      // Go to the next track.
      int nTracks = liber.musicTracks.length();

      if (nTracks > 0) {
        currentTrack = (currentTrack + 1) % nTracks;

        sound.playMusic(currentTrack);

        trackTitle.setText("%s", liber.musicTracks[currentTrack].name.c());
        trackLabel.setText("%d", currentTrack + 1);
      }
    }
  }
}

void MusicPlayer::onDraw()
{
  Frame::onDraw();

  trackTitle.draw(this);
  trackLabel.draw(this);
  volumeLabel.draw(this);
}

MusicPlayer::MusicPlayer() :
  Frame(240, 36 + style.smallFont.height(), OZ_GETTEXT("Music Player")),
  trackTitle(width / 2, 32, 0, ALIGN_HCENTRE, &style.smallFont, ""),
  trackLabel(39, 14, 0, ALIGN_CENTRE, &style.smallFont, "0"),
  volumeLabel(201, 14, 0, ALIGN_CENTRE, &style.smallFont, "5"),
  currentTrack(0), volume(5), isPlaying(false)
{
  flags |= UPDATE_BIT;

  if (liber.musicTracks.length() > 0) {
    trackLabel.setText("1");
  }
  sound.setMusicVolume(0.5f);

  add(new Button("-",  volumeDown, 20, 20),   4, 4);
  add(new Button("<<", prevTrack,  30, 20),  54, 4);
  add(new Button(">",  playTrack,  30, 20),  88, 4);
  add(new Button("x",  stopTrack,  30, 20), 122, 4);
  add(new Button(">>", nextTrack,  30, 20), 156, 4);
  add(new Button("+",  volumeUp,   20, 20), 216, 4);
}

MusicPlayer::~MusicPlayer()
{
  sound.stopMusic();
}

}
}
}
