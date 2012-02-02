/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file oz/Math.cc
 */

#include "Math.hh"

#include <cstdlib>

namespace oz
{

constexpr float Math::FLOAT_EPS;
constexpr float Math::NaN;
constexpr float Math::INF;
constexpr float Math::E;
constexpr float Math::TAU;
constexpr float Math::SQRT_2;
constexpr float Math::SQRT_3;

void Math::seed( int n )
{
  srand( uint( n ) );
}

int Math::rand( int max )
{
  return ::rand() % max;
}

float Math::rand()
{
  return float( ::rand() ) / float( RAND_MAX );
}

float Math::centralRand()
{
  return float( ::rand() - RAND_MAX / 2 ) / float( RAND_MAX / 2 );
}

float Math::normalRand()
{
  float x = float( ::rand() - RAND_MAX / 2 ) / float( RAND_MAX / 2 );
  return x*x*x;
}

}
