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

#include <matrix/Terra.hh>

#include <matrix/Liber.hh>

namespace oz
{

void Terra::reset()
{
  load(-1);
}

void Terra::load(int id_)
{
  id = id_;

  if (id < 0) {
    for (int x = 0; x < VERTS; ++x) {
      for (int y = 0; y < VERTS; ++y) {
        quads[x][y].vertex.z     = 0.0f;
        quads[x][y].normals[0] = Vec3(0.0f, 0.0f, 1.0f);
        quads[x][y].normals[1] = Vec3(0.0f, 0.0f, 1.0f);
      }
    }
  }
  else {
    const String& name = liber.terrae[id].name;
    const File&   file = liber.terrae[id].path;

    Log::print("Loading terrain '%s' ...", name.c());

    Stream is = file.read(Endian::LITTLE);

    if (is.available() == 0) {
      OZ_ERROR("Cannot read terra file '%s'", file.c());
    }

    int max = is.readInt();
    if (max != VERTS) {
      OZ_ERROR("Invalid dimension %d, should be %d", max, VERTS);
    }

    for (int x = 0; x < VERTS; ++x) {
      for (int y = 0; y < VERTS; ++y) {
        quads[x][y].vertex.z = is.readFloat();
      }
    }

    for (int x = 0; x < QUADS; ++x) {
      for (int y = 0; y < QUADS; ++y) {
        if (x != QUADS && y != QUADS) {
          const Point& a = quads[x    ][y    ].vertex;
          const Point& b = quads[x + 1][y    ].vertex;
          const Point& c = quads[x + 1][y + 1].vertex;
          const Point& d = quads[x    ][y + 1].vertex;

          quads[x][y].normals[0] = ~((c - b) ^ (a - b));
          quads[x][y].normals[1] = ~((a - d) ^ (c - d));
        }
      }
    }

    liquid = is.readInt();

    Log::printEnd(" OK");
  }
}

void Terra::init()
{
  for (int x = 0; x < VERTS; ++x) {
    for (int y = 0; y < VERTS; ++y) {
      quads[x][y].vertex.x = float(x * Quad::SIZE - DIM);
      quads[x][y].vertex.y = float(y * Quad::SIZE - DIM);
    }
  }
}

void Terra::read(const Json& json)
{
  load(liber.terraIndex(json["name"].get("")));
}

void Terra::read(Stream* is)
{
  load(liber.terraIndex(is->readString()));
}

Json Terra::write() const
{
  const char* name = id < 0 ? "" : liber.terrae[id].name.c();

  Json json(Json::OBJECT);
  json.add("name", name);
  return json;
}

void Terra::write(Stream* os) const
{
  const char* name = id < 0 ? "" : liber.terrae[id].name.c();

  os->writeString(name);
}

}
