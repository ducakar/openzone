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

/**
 * @file client/MD2Imago.hh
 */

#pragma once

#include <client/Imago.hh>
#include <client/Model.hh>

namespace oz::client
{

class MD2Imago : public Imago
{
  friend class MD2WeaponImago;

private:

  static constexpr float TURN_SMOOTHING_COEF = 0.25f;

private:

  Model*         model;
  float          h;
  MD2::AnimState anim;

private:

  explicit MD2Imago(const Bot* bot)
    : Imago(bot), anim(bot)
  {
    flags |= Imago::MD2MODEL_BIT;
  }

  explicit MD2Imago(const Vehicle* vehicle)
    : Imago(vehicle), anim(vehicle)
  {
    flags |= Imago::MD2MODEL_BIT;
  }

public:

  static Pool<MD2Imago> pool;

  static Imago* create(const Object* obj_);

  ~MD2Imago() override;

  OZ_NO_COPY(MD2Imago)
  OZ_NO_MOVE(MD2Imago)

  void draw(const Imago* parent) override;

  OZ_STATIC_POOL_ALLOC(pool)
};

}
