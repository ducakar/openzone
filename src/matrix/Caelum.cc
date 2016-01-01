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

#include <matrix/Caelum.hh>

#include <matrix/Liber.hh>

namespace oz
{

void Caelum::reset()
{
  id      = -1;
  heading = 0.0f;
  period  = 86400.0f;
  time    = 0.0f;
}

void Caelum::update()
{
  time = Math::fmod(time + Timer::TICK_TIME, period);
}

void Caelum::read(const Json& json)
{
  id      = liber.caelumIndex(json["name"].get(""));
  heading = Math::rad(json["heading"].get(0.0f));
  period  = json["period"].get(86400.0f);
  time    = json["time"].get(0.0f);
}

void Caelum::read(Stream* is)
{
  id      = is->readInt();
  heading = is->readFloat();
  period  = is->readFloat();
  time    = is->readFloat();
}

Json Caelum::write() const
{
  Json json(Json::OBJECT);

  if (id >= 0) {
    json.add("name", liber.caela[id].name);
    json.add("heading", Math::deg(heading));
    json.add("period", period);
    json.add("time", time);
  }
  return json;
}

void Caelum::write(Stream* os) const
{
  os->writeInt(id);
  os->writeFloat(heading);
  os->writeFloat(period);
  os->writeFloat(time);
}

}
