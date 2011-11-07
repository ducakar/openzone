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
 * @file matrix/FragPool.cpp
 */

#include "stable.hpp"

#include "matrix/FragPool.hpp"

#include "matrix/Frag.hpp"

namespace oz
{
namespace matrix
{

FragPool::FragPool( const char* name_ )
{
  name = name_;
}

Frag* FragPool::create( int index, const Point3& pos ) const
{
  Frag* frag = new Frag();

  frag->index = index;
  frag->p     = pos;

  return frag;
}

Frag* FragPool::create( int index, InputStream* istream ) const
{
  Frag* frag = new Frag();

  frag->index = index;

  frag->readFull( istream );

  return frag;
}

}
}
