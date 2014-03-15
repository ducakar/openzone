/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file txc_dxtn/txc_dxtn.cc
 */

#include "txc_dxtn.hh"

#include <cstdlib>

__attribute__(( noreturn ))
void fetch_2d_texel_rgb_dxt1( GLint, const GLubyte*, GLint, GLint, GLvoid* )
{
  abort();
}

__attribute__(( noreturn ))
void fetch_2d_texel_rgba_dxt1( GLint, const GLubyte*, GLint, GLint, GLvoid* )
{
  abort();
}

__attribute__(( noreturn ))
void fetch_2d_texel_rgba_dxt3( GLint, const GLubyte*, GLint, GLint, GLvoid* )
{
  abort();
}

__attribute__(( noreturn ))
void fetch_2d_texel_rgba_dxt5( GLint, const GLubyte*, GLint, GLint, GLvoid* )
{
  abort();
}

__attribute__(( noreturn ))
void tx_compress_dxtn( GLint, GLint, GLint, const GLubyte*, GLenum, GLubyte*, GLint )
{
  abort();
}
