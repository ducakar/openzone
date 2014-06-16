/*
 * ozDynamics - OpenZone Dynamics Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file ozDynamics/collision/Compound.hh
 *
 * `Compound` class.
 */

#pragma once

#include "Shape.hh"

namespace oz
{

class Compound : public Shape
{
public:

  struct Child
  {
    Shape* shape;
    Vec3   off;
    Mat3   rot;
  };

private:

  SList<Child, 16> children;  ///< Children.

public:

  static Pool<Compound> pool; ///< Memory pool.

public:

  OZ_ALWAYS_INLINE
  explicit Compound() :
    Shape(COMPOUND)
  {}

  ~Compound() override;

  OZ_ALWAYS_INLINE
  SList<Child, 16>::CIterator citer() const
  {
    return children.citer();
  }

  OZ_ALWAYS_INLINE
  const Child* begin() const
  {
    return children.begin();
  }

  OZ_ALWAYS_INLINE
  const Child* end() const
  {
    return children.end();
  }

  OZ_ALWAYS_INLINE
  const Child& operator [] (int i) const
  {
    return children[i];
  }

  OZ_ALWAYS_INLINE
  int length() const
  {
    return children.length();
  }

  void add(Shape* shape, const Vec3& off, const Mat3& rot)
  {
    Child child = { shape, off, rot };

    children.add(child);
    ++shape->nUsers;
  }

  Bounds getBounds(const Point& pos, const Mat3& rot) const override;

  OZ_STATIC_POOL_ALLOC(pool)
};

}
