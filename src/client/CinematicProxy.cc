/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file client/CinematicProxy.hh
 */

#include <client/CinematicProxy.hh>

#include <common/Timer.hh>
#include <client/LuaClient.hh>
#include <client/Camera.hh>
#include <client/Sound.hh>
#include <client/ui/GalileoFrame.hh>
#include <client/ui/MusicPlayer.hh>
#include <client/ui/UI.hh>

namespace oz
{
namespace client
{

void CinematicProxy::executeSequence(const char* path, const Lingua* missionLingua)
{
  JSON sequence;
  File file = path;

  if (!sequence.load(file)) {
    OZ_ERROR("Failed to load sequence from '%s'", file.path().cstr());
  }

  int nSteps = sequence.length();
  if (nSteps <= 0) {
    return;
  }

  Step step = {
    camera.rot, camera.p, camera.colour, Buffer(0), -1, nullptr, 0.0f, Camera::CINEMATIC
  };

  steps.reserve(nSteps);

  for (int i = 0; i < nSteps; ++i) {
    const JSON& stepConfig = sequence[i];
    const JSON& rotArray = stepConfig["rot"];

    if (!rotArray.isNull()) {
      Vec3 rot = rotArray.get(Vec3::ZERO);
      step.rot = Quat::rotationZXZ(Math::rad(rot.x), Math::rad(rot.y), Math::rad(rot.z));
    }

    const JSON& posArray = stepConfig["pos"];

    if (!posArray.isNull()) {
      step.p = posArray.get(Point::ORIGIN);
    }

    const JSON& colourArray = stepConfig["colour"];

    if (!colourArray.isNull()) {
      step.colour = colourArray.get(Mat4::ID);
    }

    const JSON& execConfig = stepConfig["exec"];
    if (execConfig.isNull()) {
      step.code.resize(0);
    }
    else {
      step.code = luaClient.compile(execConfig.get(""), path);
    }

    const JSON& trackConfig = stepConfig["track"];
    const String& track = trackConfig.get(String::EMPTY);

    if (trackConfig.isNull()) {
      step.track = -1;
    }
    else if (track.isEmpty()) {
      step.track = -2;
    }
    else {
      step.track = liber.musicTrackIndex(track);
    }

    const JSON& titleConfig = stepConfig["title"];
    const String& title = titleConfig.get(String::EMPTY);

    if (titleConfig.isNull()) {
      step.title = "";
    }
    else if (title.isEmpty()) {
      step.title = " ";
    }
    else {
      step.title = missionLingua->get(title);
    }

    step.time = stepConfig["time"].get(0.0f);

    const JSON& stateConfig = stepConfig["state"];

    if (stateConfig.isNull()) {
      step.endState = Camera::CINEMATIC;
    }
    else {
      const char* sEndState = stateConfig.get("CINEMATIC");

      if (String::equals(sEndState, "CINEMATIC")) {
        step.endState = Camera::CINEMATIC;
      }
      else if (String::equals(sEndState, "STRATEGIC")) {
        step.endState = Camera::STRATEGIC;
      }
      else if (String::equals(sEndState, "UNIT")) {
        step.endState = Camera::UNIT;
      }
      else {
        OZ_ERROR("Invalid state '%s' for sequence step; should be either CINEMATIC, STRATEGIC or"
                 " UNIT.", sEndState);
      }
    }

    steps.add(step);
  }

  sequence.clear(true);
}

void CinematicProxy::begin()
{
  ui::ui.galileoFrame->enable(false);
  ui::ui.musicPlayer->enable(false);

  cinematicText = new ui::CinematicText();
  ui::ui.root->add(cinematicText, ui::Area::CENTRE, 200);
  ui::mouse.doShow = false;

  beginRot    = camera.rot;
  beginPos    = camera.p;
  beginColour = camera.colour;

  cinematicText->set(title.substring(0, nTitleChars));
}

void CinematicProxy::end()
{
  ui::ui.root->remove(cinematicText);

  ui::ui.musicPlayer->enable(true);
  ui::ui.galileoFrame->enable(true);

  title       = "";
  nTitleChars = 0;

  stepTime    = 0.0f;

  steps.clear();
  steps.trim();
}

void CinematicProxy::prepare()
{}

void CinematicProxy::update()
{
  if (steps.isEmpty()) {
    camera.setState(Camera::State(prevState));
    return;
  }

  const Step& step = steps.first();

  if (step.endState != Camera::CINEMATIC) {
    camera.setState(Camera::State(step.endState));
    return;
  }

  float t = step.time == 0.0f ? 1.0f : min(stepTime / step.time, 1.0f);

  camera.smoothRotateTo(Quat::fastSlerp(beginRot, step.rot, t));
  camera.smoothMoveTo(Math::mix(beginPos, step.p, t));
  camera.colour = Math::mix(beginColour, step.colour, t);
  camera.align();

  if (nTitleChars < title.length()) {
    nTitleChars = min(nTitleChars + int(timer.frameTicks), title.length());

    // Take all bytes of UTF-8 characters.
    while (nTitleChars > 0 && nTitleChars < title.length() &&
           (title[nTitleChars - 1] & title[nTitleChars] & 0x80))
    {
      ++nTitleChars;
    }

    cinematicText->set(title.substring(0, nTitleChars));
  }

  stepTime += Timer::TICK_TIME;

  if (t == 1.0f) {
    beginPos    = step.p;
    beginRot    = step.rot;
    beginColour = step.colour;

    stepTime    = 0.0f;

    steps.popFirst();
  }
  else if (t == 0.0f) {
    if (!step.code.isEmpty()) {
      luaClient.execChunk(step.code.begin(), step.code.length(), "");
    }

    if (step.track == -2) {
      sound.stopMusic();
    }
    else if (step.track != -1) {
      sound.playMusic(step.track);
    }

    if (!step.title.isEmpty()) {
      title       = step.title;
      nTitleChars = 0;

      cinematicText->set(" ");
    }
  }
}

void CinematicProxy::reset()
{
  beginRot    = Quat::ID;
  beginPos    = Point::ORIGIN;
  beginColour = Mat4::ID;

  title       = "";
  nTitleChars = 0;

  stepTime    = 0.0f;

  camera.colour = camera.baseColour;

  steps.clear();
  steps.trim();
}

void CinematicProxy::read(InputStream* is)
{
  beginRot    = is->readQuat();
  beginPos    = is->readPoint();
  beginColour = is->readMat4();

  title       = is->readString();
  nTitleChars = is->readInt();

  stepTime    = is->readFloat();
  prevState   = is->readInt();

  int nSteps = is->readInt();
  for (int i = 0; i < nSteps; ++i) {
    Step step;

    step.rot      = is->readQuat();
    step.p        = is->readPoint();
    step.colour   = is->readMat4();

    step.track    = is->readInt();
    step.title    = is->readString();

    step.time     = is->readFloat();
    step.endState = is->readInt();

    steps.add(step);
  }
}

void CinematicProxy::read(const JSON&)
{}

void CinematicProxy::write(OutputStream* os) const
{
  os->writeQuat(beginRot);
  os->writePoint(beginPos);
  os->writeMat4(beginColour);

  os->writeString(title);
  os->writeInt(nTitleChars);

  os->writeFloat(stepTime);
  os->writeInt(prevState);

  os->writeInt(steps.length());
  for (int i = 0; i < steps.length(); ++i) {
    const Step& step = steps[i];

    os->writeQuat(step.rot);
    os->writePoint(step.p);
    os->writeMat4(step.colour);

    os->writeInt(step.track);
    os->writeString(step.title);

    os->writeFloat(step.time);
    os->writeInt(step.endState);
  }
}

JSON CinematicProxy::write() const
{
  return JSON::NIL;
}

}
}
