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

#include <client/CinematicProxy.hh>

#include <client/LuaClient.hh>
#include <client/Camera.hh>
#include <client/Sound.hh>
#include <client/Input.hh>
#include <client/ui/GalileoFrame.hh>
#include <client/ui/MusicPlayer.hh>
#include <client/ui/UI.hh>

namespace oz
{
namespace client
{

void CinematicProxy::executeSequence(const File& file, const Lingua* missionLingua)
{
  Json sequence;

  if (!sequence.load(file)) {
    OZ_ERROR("Failed to load sequence from '%s'", file.c());
  }

  int nSteps = sequence.length();
  if (nSteps <= 0) {
    return;
  }

  Step step = {
    camera.rot, camera.p, camera.colour, String::EMPTY, -1, "", 0.0f, false, Camera::CINEMATIC
  };

  steps.reserve(nSteps, true);

  for (int i = 0; i < nSteps; ++i) {
    const Json& stepConfig = sequence[i];
    const Json& rotArray = stepConfig["rot"];

    if (!rotArray.isNull()) {
      Vec3 rot = rotArray.get(Vec3::ZERO);
      step.rot = Quat::rotationZXZ(Math::rad(rot.x), Math::rad(rot.y), Math::rad(rot.z));
    }

    const Json& posArray = stepConfig["pos"];

    if (!posArray.isNull()) {
      step.p = posArray.get(Point::ORIGIN);
    }

    const Json& colourArray = stepConfig["colour"];

    if (!colourArray.isNull()) {
      step.colour = colourArray.get(Mat4::ID);
    }

    step.code = stepConfig["exec"].get("");

    const Json& trackConfig = stepConfig["track"];
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

    const Json& titleConfig = stepConfig["title"];
    const String& title = titleConfig.get(String::EMPTY);

    if (titleConfig.isNull()) {
      step.title = "";
    }
    else if (title.isEmpty()) {
      step.title = "";
    }
    else {
      step.title = missionLingua->get(title);
    }

    step.time = stepConfig["time"].get(0.0f);
    step.isSkippable = stepConfig["isSkippable"].get(false);

    const Json& stateConfig = stepConfig["state"];

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
  ui::mouse.isVisible = false;

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
    nTitleChars = min(nTitleChars + 1, title.length());

    // Take all bytes of a UTF-8 character.
    while (nTitleChars < title.length() && (title[nTitleChars - 1] & title[nTitleChars] & 0x80)) {
      ++nTitleChars;
    }

    cinematicText->set(title.substring(0, nTitleChars));
  }

  stepTime += Timer::TICK_TIME;

  if (step.isSkippable && input.keys[Input::KEY_SKIP] && !input.oldKeys[Input::KEY_SKIP]) {
    beginPos    = camera.p;
    beginRot    = camera.rot;
    beginColour = camera.colour;

    stepTime    = 0.0f;

    while (!steps.isEmpty() && steps.first().isSkippable) {
      steps.popFirst();
    }
  }
  else if (t == 1.0f) {
    beginPos    = step.p;
    beginRot    = step.rot;
    beginColour = step.colour;

    stepTime    = 0.0f;

    steps.popFirst();
  }
  else if (t == 0.0f) {
    if (!step.code.isEmpty()) {
      luaClient.exec(step.code);
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

      cinematicText->set("");
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

void CinematicProxy::read(Stream* is)
{
  beginRot    = is->read<Quat>();
  beginPos    = is->read<Point>();
  beginColour = is->read<Mat4>();

  title       = is->readString();
  nTitleChars = is->readInt();

  stepTime    = is->readFloat();
  prevState   = is->readInt();

  int nSteps = is->readInt();
  for (int i = 0; i < nSteps; ++i) {
    Step step;

    step.rot         = is->read<Quat>();
    step.p           = is->read<Point>();
    step.colour      = is->read<Mat4>();

    step.track       = is->readInt();
    step.title       = is->readString();

    step.time        = is->readFloat();
    step.isSkippable = is->readBool();
    step.endState    = is->readInt();

    steps.add(step);
  }
}

void CinematicProxy::read(const Json&)
{}

void CinematicProxy::write(Stream* os) const
{
  os->write<Quat>(beginRot);
  os->write<Point>(beginPos);
  os->write<Mat4>(beginColour);

  os->writeString(title);
  os->writeInt(nTitleChars);

  os->writeFloat(stepTime);
  os->writeInt(prevState);

  os->writeInt(steps.length());
  for (int i = 0; i < steps.length(); ++i) {
    const Step& step = steps[i];

    os->write<Quat>(step.rot);
    os->write<Point>(step.p);
    os->write<Mat4>(step.colour);

    os->writeInt(step.track);
    os->writeString(step.title);

    os->writeFloat(step.time);
    os->writeBool(step.isSkippable);
    os->writeInt(step.endState);
  }
}

Json CinematicProxy::write() const
{
  return Json::NIL;
}

}
}
