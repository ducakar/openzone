/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

#include <matrix/FragPool.hh>

#include <matrix/Frag.hh>
#include <matrix/Liber.hh>

namespace oz
{

FragPool::FragPool(const Json& config, const char* name_, int id_)
  : name(name_), id(id_)
{
  flags = 0;

  OZ_CLASS_FLAG(oz::FragPool::FADEOUT_BIT, "flag.fadeout", true);

  velocitySpread = config["velocitySpread"].get(4.0f);

  if (velocitySpread < 0.0f) {
    OZ_ERROR("%s: Frag velocitySpread must be >= 0.0", name_);
  }

  life       = config["life"].get(4.0f);
  lifeSpread = config["lifeSpread"].get(1.0f);
  mass       = config["mass"].get(0.0f);
  elasticity = config["elasticity"].get(0.5f);

  if (life <= 0.0f) {
    OZ_ERROR("%s: Frag life must be > 0.0", name_);
  }
  if (lifeSpread < 0.0f) {
    OZ_ERROR("%s: Frag lifeSpread must be >= 0.0", name_);
  }
  if (mass < 0.0f) {
    OZ_ERROR("%s: Frag mass must be >= 0.0", name_);
  }
  if (elasticity < 0.0f || 1.0f < elasticity) {
    OZ_ERROR("%s: Frag elasticity must lie on interval [0, 1]", name_);
  }

  const Json& modelsConfig = config["models"];
  int nModels = modelsConfig.size();

  for (int i = 0; i < nModels; ++i) {
    const char* modelName = modelsConfig[i].get("");

    if (String::isEmpty(modelName)) {
      OZ_ERROR("%s: Empty name for model #%d", name_, i);
    }

    models.add(liber.modelIndex(modelName));
  }
}

Frag* FragPool::create(int index, const Point& pos, const Vec3& velocity) const
{
  return new Frag(this, index, pos, velocity);
}

Frag* FragPool::create(Stream* is) const
{
  return new Frag(this, is);
}

}
