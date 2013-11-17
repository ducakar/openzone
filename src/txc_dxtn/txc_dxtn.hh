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
 * @file txc_dxtn/txc_dxtn.hh
 *
 * Dummy txc_dxtn library.
 *
 * We don't need full S3TC support, only ability to upload compressed textures. However, when Mesa
 * doesn't find libtxc_dxtn library, it disables all S3TC-related stuff, including those things that
 * could work without libtxc_dxtn.
 *
 * This dummy txc_dxtn library should be put into `LD_LIBRARY_PATH` when starting OpenZone on a
 * Linux system with open-source (Mesa) OpenGL implementation when there's no txc_dxtn library
 * present. It should fool Mesa not to disable S3TC functionality that could work without txc_dxtn.
 *
 * If the program tries to use any S3TC-related functionality that requires txc_dxtn, any of these
 * functions will crash the program with `abort()`.
 */

#pragma once

#include <GL/gl.h>

extern "C"
void fetch_2d_texel_rgb_dxt1( GLint stride, const GLubyte* data, GLint x, GLint y, GLvoid* texel );

extern "C"
void fetch_2d_texel_rgba_dxt1( GLint stride, const GLubyte* data, GLint x, GLint y, GLvoid* texel );

extern "C"
void fetch_2d_texel_rgba_dxt3( GLint stride, const GLubyte* data, GLint x, GLint y, GLvoid* texel );

extern "C"
void fetch_2d_texel_rgba_dxt5( GLint stride, const GLubyte* data, GLint x, GLint y, GLvoid* texel );

extern "C"
void tx_compress_dxtn( GLint components, GLint width, GLint height, const GLubyte* data,
                       GLenum destFormat, GLubyte* destData, GLint destStride );
