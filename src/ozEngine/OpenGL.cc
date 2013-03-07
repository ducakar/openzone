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
 * @file ozEngine/OpenGL.cc
 */

#include "OpenGL.hh"

#include <SDL.h>

#ifdef _WIN32
# define OZ_DL_GLLOAD( func ) \
  *( void** )( &func ) = SDL_GL_GetProcAddress( #func ); \
  if( func == nullptr ) { \
    OZ_ERROR( "Failed to link OpenGL function: " #func ); \
  }
#endif

namespace oz
{

#ifdef _WIN32

OZ_DL_DEFINE( glUniform1i                  );
OZ_DL_DEFINE( glUniform2i                  );
OZ_DL_DEFINE( glUniform3i                  );
OZ_DL_DEFINE( glUniform4i                  );
OZ_DL_DEFINE( glUniform1iv                 );
OZ_DL_DEFINE( glUniform2iv                 );
OZ_DL_DEFINE( glUniform3iv                 );
OZ_DL_DEFINE( glUniform4iv                 );
OZ_DL_DEFINE( glUniform1f                  );
OZ_DL_DEFINE( glUniform2f                  );
OZ_DL_DEFINE( glUniform3f                  );
OZ_DL_DEFINE( glUniform4f                  );
OZ_DL_DEFINE( glUniform1fv                 );
OZ_DL_DEFINE( glUniform2fv                 );
OZ_DL_DEFINE( glUniform3fv                 );
OZ_DL_DEFINE( glUniform4fv                 );
OZ_DL_DEFINE( glUniformMatrix2fv           );
OZ_DL_DEFINE( glUniformMatrix3fv           );
OZ_DL_DEFINE( glUniformMatrix4fv           );

OZ_DL_DEFINE( glEnableVertexAttribArray    );
OZ_DL_DEFINE( glVertexAttribPointer        );
OZ_DL_DEFINE( glDrawRangeElements          );

OZ_DL_DEFINE( glGenBuffers                 );
OZ_DL_DEFINE( glDeleteBuffers              );
OZ_DL_DEFINE( glBindBuffer                 );
OZ_DL_DEFINE( glBufferData                 );
OZ_DL_DEFINE( glMapBuffer                  );
OZ_DL_DEFINE( glUnmapBuffer                );

OZ_DL_DEFINE( glCreateShader               );
OZ_DL_DEFINE( glDeleteShader               );
OZ_DL_DEFINE( glShaderSource               );
OZ_DL_DEFINE( glCompileShader              );
OZ_DL_DEFINE( glGetShaderiv                );
OZ_DL_DEFINE( glGetShaderInfoLog           );
OZ_DL_DEFINE( glCreateProgram              );
OZ_DL_DEFINE( glDeleteProgram              );
OZ_DL_DEFINE( glAttachShader               );
OZ_DL_DEFINE( glDetachShader               );
OZ_DL_DEFINE( glLinkProgram                );
OZ_DL_DEFINE( glGetProgramiv               );
OZ_DL_DEFINE( glGetProgramInfoLog          );
OZ_DL_DEFINE( glGetUniformLocation         );
OZ_DL_DEFINE( glBindAttribLocation         );
OZ_DL_DEFINE( glUseProgram                 );

OZ_DL_DEFINE( glActiveTexture              );
OZ_DL_DEFINE( glCompressedTexImage2D       );
OZ_DL_DEFINE( glGetCompressedTexImage      );

OZ_DL_DEFINE( glGenerateMipmapEXT          );
OZ_DL_DEFINE( glGenRenderbuffersEXT        );
OZ_DL_DEFINE( glDeleteRenderbuffersEXT     );
OZ_DL_DEFINE( glBindRenderbufferEXT        );
OZ_DL_DEFINE( glRenderbufferStorageEXT     );
OZ_DL_DEFINE( glGenFramebuffersEXT         );
OZ_DL_DEFINE( glDeleteFramebuffersEXT      );
OZ_DL_DEFINE( glBindFramebufferEXT         );
OZ_DL_DEFINE( glFramebufferRenderbufferEXT );
OZ_DL_DEFINE( glFramebufferTexture2DEXT    );
OZ_DL_DEFINE( glCheckFramebufferStatusEXT  );

#endif

#ifndef NDEBUG

void glCheckError( const char* function, const char* file, int line )
{
  const char* message;
  GLenum result = glGetError();

  switch( result ) {
    case GL_NO_ERROR: {
      return;
    }
    case GL_INVALID_ENUM: {
      message = "GL_INVALID_ENUM";
      break;
    }
    case GL_INVALID_VALUE: {
      message = "GL_INVALID_VALUE";
      break;
    }
    case GL_INVALID_OPERATION: {
      message = "GL_INVALID_OPERATION";
      break;
    }
# ifdef GL_STACK_OVERFLOW
    case GL_STACK_OVERFLOW: {
      message = "GL_STACK_OVERFLOW";
      break;
    }
# endif
# ifdef GL_STACK_UNDERFLOW
    case GL_STACK_UNDERFLOW: {
      message = "GL_STACK_UNDERFLOW";
      break;
    }
# endif
    case GL_OUT_OF_MEMORY: {
      message = "GL_OUT_OF_MEMORY";
      break;
    }
# ifdef GL_TABLE_TOO_LARGE
    case GL_TABLE_TOO_LARGE: {
      message = "GL_TABLE_TOO_LARGE";
      break;
    }
# endif
    default: {
      message = String::str( "UNKNOWN(%d)", int( result ) );
      break;
    }
  }

  System::error( function, file, line, 1, "GL error '%s'", message );
}

#endif

void glInit()
{
#ifdef _WIN32

  OZ_DL_GLLOAD( glUniform1i                  );
  OZ_DL_GLLOAD( glUniform2i                  );
  OZ_DL_GLLOAD( glUniform3i                  );
  OZ_DL_GLLOAD( glUniform4i                  );
  OZ_DL_GLLOAD( glUniform1iv                 );
  OZ_DL_GLLOAD( glUniform2iv                 );
  OZ_DL_GLLOAD( glUniform3iv                 );
  OZ_DL_GLLOAD( glUniform4iv                 );
  OZ_DL_GLLOAD( glUniform1f                  );
  OZ_DL_GLLOAD( glUniform2f                  );
  OZ_DL_GLLOAD( glUniform3f                  );
  OZ_DL_GLLOAD( glUniform4f                  );
  OZ_DL_GLLOAD( glUniform1fv                 );
  OZ_DL_GLLOAD( glUniform2fv                 );
  OZ_DL_GLLOAD( glUniform3fv                 );
  OZ_DL_GLLOAD( glUniform4fv                 );
  OZ_DL_GLLOAD( glUniformMatrix2fv           );
  OZ_DL_GLLOAD( glUniformMatrix3fv           );
  OZ_DL_GLLOAD( glUniformMatrix4fv           );

  OZ_DL_GLLOAD( glEnableVertexAttribArray    );
  OZ_DL_GLLOAD( glVertexAttribPointer        );
  OZ_DL_GLLOAD( glDrawRangeElements          );

  OZ_DL_GLLOAD( glGenBuffers                 );
  OZ_DL_GLLOAD( glDeleteBuffers              );
  OZ_DL_GLLOAD( glBindBuffer                 );
  OZ_DL_GLLOAD( glBufferData                 );
  OZ_DL_GLLOAD( glMapBuffer                  );
  OZ_DL_GLLOAD( glUnmapBuffer                );

  OZ_DL_GLLOAD( glCreateShader               );
  OZ_DL_GLLOAD( glDeleteShader               );
  OZ_DL_GLLOAD( glShaderSource               );
  OZ_DL_GLLOAD( glCompileShader              );
  OZ_DL_GLLOAD( glGetShaderiv                );
  OZ_DL_GLLOAD( glGetShaderInfoLog           );
  OZ_DL_GLLOAD( glCreateProgram              );
  OZ_DL_GLLOAD( glDeleteProgram              );
  OZ_DL_GLLOAD( glAttachShader               );
  OZ_DL_GLLOAD( glDetachShader               );
  OZ_DL_GLLOAD( glLinkProgram                );
  OZ_DL_GLLOAD( glGetProgramiv               );
  OZ_DL_GLLOAD( glGetProgramInfoLog          );
  OZ_DL_GLLOAD( glGetUniformLocation         );
  OZ_DL_GLLOAD( glBindAttribLocation         );
  OZ_DL_GLLOAD( glUseProgram                 );

  OZ_DL_GLLOAD( glActiveTexture              );
  OZ_DL_GLLOAD( glCompressedTexImage2D       );
  OZ_DL_GLLOAD( glGetCompressedTexImage      );

  OZ_DL_GLLOAD( glGenerateMipmapEXT          );
  OZ_DL_GLLOAD( glGenRenderbuffersEXT        );
  OZ_DL_GLLOAD( glDeleteRenderbuffersEXT     );
  OZ_DL_GLLOAD( glBindRenderbufferEXT        );
  OZ_DL_GLLOAD( glRenderbufferStorageEXT     );
  OZ_DL_GLLOAD( glGenFramebuffersEXT         );
  OZ_DL_GLLOAD( glDeleteFramebuffersEXT      );
  OZ_DL_GLLOAD( glBindFramebufferEXT         );
  OZ_DL_GLLOAD( glFramebufferRenderbufferEXT );
  OZ_DL_GLLOAD( glFramebufferTexture2DEXT    );
  OZ_DL_GLLOAD( glCheckFramebufferStatusEXT  );

#endif
}

}
