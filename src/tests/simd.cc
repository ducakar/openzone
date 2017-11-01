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

// #define OZ_SIMD
#include <ozCore/ozCore.hh>

#include <cstdio>

using float4 = float __attribute__((vector_size(16)));

using namespace oz;

#if 1
# define VecX Vec3
# define CreateVecX(x, y, z, w) Vec3(x, y, z)
#else
# define VecX Vec4
# define CreateVecX(x, y, z, w) Vec4(x, y, z, w)
#endif

static const int MAX = 10000;

static VecX a[MAX];
static VecX b[MAX];
static VecX c[MAX];
static VecX d[MAX];
static VecX e[MAX];

static Mat4 ma[MAX];
static Mat4 mb[MAX];
static Mat4 mc[MAX];

int main()
{
  for (int i = 0; i < MAX; ++i) {
    a[i] = CreateVecX(Math::rand(), Math::rand(), Math::rand(), Math::rand());
    b[i] = CreateVecX(Math::rand(), Math::rand(), Math::rand(), Math::rand());
    c[i] = CreateVecX(Math::rand(), Math::rand(), Math::rand(), Math::rand());
    d[i] = CreateVecX(Math::rand(), Math::rand(), Math::rand(), Math::rand());
  }

  for (int i = 0; i < MAX; ++i) {
    ma[i] = Mat4(Math::rand(), Math::rand(), Math::rand(), Math::rand(),
                 Math::rand(), Math::rand(), Math::rand(), Math::rand(),
                 Math::rand(), Math::rand(), Math::rand(), Math::rand(),
                 Math::rand(), Math::rand(), Math::rand(), Math::rand());
    mb[i] = Mat4(Math::rand(), Math::rand(), Math::rand(), Math::rand(),
                 Math::rand(), Math::rand(), Math::rand(), Math::rand(),
                 Math::rand(), Math::rand(), Math::rand(), Math::rand(),
                 Math::rand(), Math::rand(), Math::rand(), Math::rand());
  }

  Instant t0 = Instant::now();

  for (int k = 0; k < 10000; ++k) {
    for (int i = 0; i < MAX; ++i) {
      d[i]  += c[i] * (a[i] * b[i]);
      e[i]   = d[i] + c[i] - abs(a[i] + b[i]);
      a[i]  += CreateVecX(0.1f, 0, 0, 0);
      b[i].y = a[i].y;
      b[i].z = a[i].z + c[i].x;
    }
  }

  Log() << "Vectors: " << (Instant::now() - t0).ms();

  t0 = Instant::now();

  for (int k = 0; k < 10000; ++k) {
    for (int i = 0; i < MAX; ++i) {
      ma[i] -= mb[i];
      mc[i]  = ma[i] * mb[i];
      mc[i] *= 10.0f;
    }
  }

  Log() << "Matrices: " << (Instant::now() - t0).ms();
  return 0;
}
