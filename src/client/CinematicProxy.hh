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

#pragma once

#include <client/Proxy.hh>
#include <client/ui/CinematicText.hh>

namespace oz
{
namespace client
{

class CinematicProxy : public Proxy
{
private:

  struct Step
  {
    enum Type
    {
      SWITCH,
      WAIT,
      MOVE
    };

    Quat   rot;
    Point  p;
    Mat4   colour;

    String code;
    int    track;
    String title;

    float  time;
    bool   isSkippable;
    int    endState;
  };

  List<Step> steps;

  Quat       beginRot;
  Point      beginPos;
  Mat4       beginColour;

  String     title;
  int        nTitleChars;

  float      stepTime;
  int        prevState;

  ui::CinematicText* cinematicText;

public:

  void executeSequence(const File& file, const Lingua* lingua);

  void begin() override;
  void end() override;

  void prepare() override;
  void update() override;

  void reset() override;

  void read(Stream* is) override;
  void read(const Json& json) override;

  void write(Stream* os) const override;
  Json write() const override;

};

}
}
