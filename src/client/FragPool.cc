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

#include <client/FragPool.hh>

#include <client/Shader.hh>
#include <client/Context.hh>
#include <client/Model.hh>

namespace oz::client
{

FragPool::FragPool(const oz::FragPool* pool_)
  : pool(pool_), flags(pool_->flags)
{
  models.reserve(pool->models.size(), true);

  for (int model : pool->models) {
    models.add(context.requestModel(model));
  }
}

FragPool::~FragPool()
{
  for (int model : pool->models) {
    context.releaseModel(model);
  }
}

void FragPool::draw(const Frag* frag)
{
  int    index = frag->index % models.size();
  Model* model = models[index];

  if (model->isLoaded()) {
    tf.model = Mat4::translation(frag->p - Point::ORIGIN);
    tf.model.rotateX(frag->p.x);
    tf.model.rotateY(frag->p.y);
    tf.model.rotateZ(frag->p.z);

    tf.colour.w.w = flags & FADEOUT_BIT ? clamp(frag->life, 0.0f, 1.0f) : 1.0f;

    model->schedule(0, Model::SCENE_QUEUE);

    tf.colour.w.w = 1.0f;
  }
}

}
