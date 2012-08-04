/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file txc_dxtn/txc_dxtn.c
 */

#include "txc_dxtn.h"

#include <stdlib.h>

void fetch_2d_texel_rgb_dxt1( GLint stride, const GLubyte* data, GLint x, GLint y, GLvoid* texel )
{
  abort();
}

void fetch_2d_texel_rgba_dxt1( GLint stride, const GLubyte* data, GLint x, GLint y, GLvoid* texel )
{
  abort();
}

void fetch_2d_texel_rgba_dxt3( GLint stride, const GLubyte* data, GLint x, GLint y, GLvoid* texel )
{
  abort();
}

void fetch_2d_texel_rgba_dxt5( GLint stride, const GLubyte* data, GLint x, GLint y, GLvoid* texel )
{
  abort();
}

void tx_compress_dxtn( GLint components, GLint width, GLint height, const GLubyte* data,
                       GLenum destFormat, GLubyte* destData, GLint destStride )
{
  abort();
}
