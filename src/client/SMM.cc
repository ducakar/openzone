/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file client/SMM.cc
 */

#include <client/SMM.hh>

#include <client/Context.hh>

namespace oz
{
namespace client
{

SMM::SMM( int id_ ) :
  id( id_ )
{}

SMM* SMM::create( int id )
{
  return new SMM( id );
}

SMM::~SMM()
{}

void SMM::preload()
{
  mesh.preload( liber.models[id].path );
}

void SMM::load()
{
  mesh.load( GL_STATIC_DRAW );
}

}
}
