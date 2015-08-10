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
 * @file client/PartClass.cc
 */

#include <client/PartClass.hh>

#include <client/Context.hh>

namespace oz
{
namespace client
{

bool PartClass::isLoaded() const
{
  return false;
}

// void PartClass::init(InputStream* is)
// {
//   flags          = 0;
//
//   nParts         = is->readInt();
//   velocity       = is->read<Vec3>();
//   velocitySpread = is->readFloat();
//   texId          = liber.textureIndex(is->readString());
//   endTexId       = liber.textureIndex(is->readString());
// }

void PartClass::load()
{
  context.requestTexture(texId);
  context.requestTexture(endTexId);

  flags |= LOADED_BIT;
}

void PartClass::unload()
{
  if (flags & LOADED_BIT) {
    context.releaseTexture(texId);
    context.releaseTexture(endTexId);
  }
}

}
}
