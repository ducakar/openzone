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
 * @file client/OpenGL.hh
 */

#pragma once

#include <client/NaCl.hh>

#ifdef OZ_GL_ES
# include <GLES2/gl2.h>
#else
# define GL_GLEXT_PROTOTYPES
# include <SDL_opengl.h>
#endif

// Floating-point texture formats.
#ifndef GL_VERSION_3_0
# define GL_RGBA32F                        0x8814
# define GL_RGB32F                         0x8815
# define GL_RGBA16F                        0x881A
# define GL_RGB16F                         0x881B
#endif

// S3 texture compression formats.
#ifndef GL_EXT_texture_compression_s3tc
# define GL_EXT_texture_compression_s3tc
# define GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
# define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
# define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
# define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
#endif

namespace oz
{
namespace client
{

#ifdef _WIN32

extern OZ_DL_DECLARE( glUniform1i                  );
extern OZ_DL_DECLARE( glUniform2i                  );
extern OZ_DL_DECLARE( glUniform3i                  );
extern OZ_DL_DECLARE( glUniform4i                  );
extern OZ_DL_DECLARE( glUniform1iv                 );
extern OZ_DL_DECLARE( glUniform2iv                 );
extern OZ_DL_DECLARE( glUniform3iv                 );
extern OZ_DL_DECLARE( glUniform4iv                 );
extern OZ_DL_DECLARE( glUniform1f                  );
extern OZ_DL_DECLARE( glUniform2f                  );
extern OZ_DL_DECLARE( glUniform3f                  );
extern OZ_DL_DECLARE( glUniform4f                  );
extern OZ_DL_DECLARE( glUniform1fv                 );
extern OZ_DL_DECLARE( glUniform2fv                 );
extern OZ_DL_DECLARE( glUniform3fv                 );
extern OZ_DL_DECLARE( glUniform4fv                 );
extern OZ_DL_DECLARE( glUniformMatrix2fv           );
extern OZ_DL_DECLARE( glUniformMatrix3fv           );
extern OZ_DL_DECLARE( glUniformMatrix4fv           );

extern OZ_DL_DECLARE( glEnableVertexAttribArray    );
extern OZ_DL_DECLARE( glVertexAttribPointer        );
extern OZ_DL_DECLARE( glDrawRangeElements          );

extern OZ_DL_DECLARE( glGenBuffers                 );
extern OZ_DL_DECLARE( glDeleteBuffers              );
extern OZ_DL_DECLARE( glBindBuffer                 );
extern OZ_DL_DECLARE( glBufferData                 );
extern OZ_DL_DECLARE( glMapBuffer                  );
extern OZ_DL_DECLARE( glUnmapBuffer                );

extern OZ_DL_DECLARE( glCreateShader               );
extern OZ_DL_DECLARE( glDeleteShader               );
extern OZ_DL_DECLARE( glShaderSource               );
extern OZ_DL_DECLARE( glCompileShader              );
extern OZ_DL_DECLARE( glGetShaderiv                );
extern OZ_DL_DECLARE( glGetShaderInfoLog           );
extern OZ_DL_DECLARE( glCreateProgram              );
extern OZ_DL_DECLARE( glDeleteProgram              );
extern OZ_DL_DECLARE( glAttachShader               );
extern OZ_DL_DECLARE( glDetachShader               );
extern OZ_DL_DECLARE( glLinkProgram                );
extern OZ_DL_DECLARE( glGetProgramiv               );
extern OZ_DL_DECLARE( glGetProgramInfoLog          );
extern OZ_DL_DECLARE( glGetUniformLocation         );
extern OZ_DL_DECLARE( glBindAttribLocation         );
extern OZ_DL_DECLARE( glUseProgram                 );

extern OZ_DL_DECLARE( glActiveTexture              );
extern OZ_DL_DECLARE( glCompressedTexImage2D       );
extern OZ_DL_DECLARE( glGetCompressedTexImage      );

extern OZ_DL_DECLARE( glGenerateMipmapEXT          );
extern OZ_DL_DECLARE( glGenRenderbuffersEXT        );
extern OZ_DL_DECLARE( glDeleteRenderbuffersEXT     );
extern OZ_DL_DECLARE( glBindRenderbufferEXT        );
extern OZ_DL_DECLARE( glRenderbufferStorageEXT     );
extern OZ_DL_DECLARE( glGenFramebuffersEXT         );
extern OZ_DL_DECLARE( glDeleteFramebuffersEXT      );
extern OZ_DL_DECLARE( glBindFramebufferEXT         );
extern OZ_DL_DECLARE( glFramebufferRenderbufferEXT );
extern OZ_DL_DECLARE( glFramebufferTexture2DEXT    );
extern OZ_DL_DECLARE( glCheckFramebufferStatusEXT  );

#endif

#ifdef NDEBUG
# define OZ_GL_CHECK_ERROR() void( 0 )
#else
# define OZ_GL_CHECK_ERROR() oz::client::glCheckError( __PRETTY_FUNCTION__, __FILE__, __LINE__ )

void glCheckError( const char* function, const char* file, int line );

#endif

void glInit();

}
}
