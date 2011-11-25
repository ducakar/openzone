/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/OpenAL.hpp
 */

#pragma once

#include "client/common.hpp"

#include <AL/al.h>

namespace oz
{
namespace client
{

#ifdef NDEBUG
# define OZ_AL_CHECK_ERROR() void( 0 )
#else
# define OZ_AL_CHECK_ERROR() oz::client::alCheckError( __FILE__, __LINE__, __PRETTY_FUNCTION__ )

void alCheckError( const char* file, int line, const char* function );

#endif

}
}
