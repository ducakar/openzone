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
 * @file client/Imago.hh
 *
 * Imago class.
 */

#pragma once

#include <client/common.hh>

namespace oz::client
{

class Imago
{
public:

  using CreateFunc = Imago* (const Object* object);

  static constexpr int UPDATED_BIT  = 0x00000001;
  static constexpr int MD2MODEL_BIT = 0x00000002;

protected:

  const Object*      obj;
  const ObjectClass* clazz;

public:

  int flags = 0;

protected:

  explicit Imago(const Object* obj_)
    : obj(obj_), clazz(obj_->clazz)
  {}

public:

  virtual ~Imago();

  OZ_NO_COPY(Imago)
  OZ_NO_MOVE(Imago)

  virtual void draw(const Imago* parent) = 0;

};

}
