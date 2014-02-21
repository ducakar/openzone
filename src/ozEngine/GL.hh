/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file ozEngine/GL.hh
 *
 * `GL` class and wrapper for OpenGL headers.
 */

#pragma once

#include "common.hh"

#ifndef DOXYGEN_IGNORE

#ifdef OZ_GL_ES
# include <GLES2/gl2.h>
#else
# define GL_GLEXT_PROTOTYPES
# include <SDL_opengl.h>
#endif

// S3 texture compression formats.
#ifndef GL_EXT_texture_compression_s3tc
# define GL_EXT_texture_compression_s3tc
# define GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
# define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
# define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
# define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
#endif

#endif // DOXYGEN_IGNORE

/**
 * @def OZ_GL_CHECK_ERROR
 * In debug mode, check for OpenGL errors and crash with some diagnostics if there is one.
 */
#ifdef NDEBUG
# define OZ_GL_CHECK_ERROR() void( 0 )
#else
# define OZ_GL_CHECK_ERROR() oz::GL::checkError( __PRETTY_FUNCTION__, __FILE__, __LINE__ )
#endif

namespace oz
{

#ifdef _WIN32

/*
 * OpenGL 2.0+ functions cannot be linked on executable load, we must link them in runtime.
 */
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

extern OZ_DL_DECLARE( glDrawBuffers                );

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

/**
 * OpenGL utilities.
 */
class GL
{
public:

  /**
   * Helper method for `OZ_GL_CHECK_ERROR` macro.
   */
  static void checkError( const char* function, const char* file, int line );

  /**
   * Load texture image from a (DirectX 9) DDS file.
   *
   * This function is to be used in place of `glTexImage2D()`.
   *
   * For non-mipmapped textures `GL_LINEAR` & `GL_LINEAR` filters and `GL_CLAMP_TO_EDGE` wrapping
   * are used as default, while for mipmapped textures `GL_LINEAR` & `GL_LINEAR_MIPMAP_LINEAR`
   * filters and `GL_REPEAT` wrapping (or `GL_CLAMP_TO_EDGE` if a cube map) are used.
   *
   * The source file must be in either BGR, BGRA, DXT1, DXT3 or DXT5 format.
   *
   * @param file source file.
   * @param bias the number of mipmaps at the beginning that should be skipped to obtain lower
   *        texture quality.
   *
   * @return number of mipmap levels loaded, 0 on an error.
   */
  static int textureDataFromFile( const File& file, int bias = 0 );

  /**
   * Compile a GLSL shader from a file.
   *
   * This function is to be used in place of `glShaderSource()`/`glCompileShader()`.
   *
   * If an include directive is encountered in a file, the included file is inserted before the
   * current one in the source files list passed to `glShaderSource()`. The line with include
   * directive is replaced by an empty line.
   *
   * The given `defines` string should contain newline separated list of define directives that are
   * processed before the rest of GLSL source. It is passed as the first entry in the source files
   * list to `glShaderSource()`.
   *
   * @param shader OpenGL shader id.
   * @param defines a string containing defines used during shader compilation.
   * @param file main source file.
   */
  static bool compileShaderFromFile( GLuint shader, const char* defines, const File& file );

  /**
   * Link previously declared OpenGL functions on Windows, NOP on other platforms.
   */
  static void init();

};

}
