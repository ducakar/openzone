/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file oz/Math.cpp
 */

#include "Math.hpp"

#include <cmath>
#include <cfloat>
#include <cstdlib>

namespace oz
{

const float Math::EPSILON = FLT_EPSILON;
const float Math::E       = 2.718281828459045f;
const float Math::TAU     = 6.283185307179586f;
const float Math::INF     = float( INFINITY );
const float Math::NaN     = float( NAN );

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

}
