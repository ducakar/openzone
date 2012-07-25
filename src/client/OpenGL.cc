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
 * @file client/OpenGL.cc
 */

#include "stable.hh"

#include "client/OpenGL.hh"

#ifdef _WIN32
# define OZ_REGISTER_GLFUNC( func ) \
  *( void** )( &func ) = SDL_GL_GetProcAddress( #func ); \
  if( func == null ) { \
    throw Exception( "Failed to link OpenGL function '" #func "'" ); \
  }
#endif

namespace oz
{
namespace client
{

#ifdef _WIN32
PFNGLUNIFORM1IPROC               glUniform1i;
PFNGLUNIFORM2IPROC               glUniform2i;
PFNGLUNIFORM3IPROC               glUniform3i;
PFNGLUNIFORM4IPROC               glUniform4i;
PFNGLUNIFORM1IVPROC              glUniform1iv;
PFNGLUNIFORM2IVPROC              glUniform2iv;
PFNGLUNIFORM3IVPROC              glUniform3iv;
PFNGLUNIFORM4IVPROC              glUniform4iv;
PFNGLUNIFORM1FPROC               glUniform1f;
PFNGLUNIFORM2FPROC               glUniform2f;
PFNGLUNIFORM3FPROC               glUniform3f;
PFNGLUNIFORM4FPROC               glUniform4f;
PFNGLUNIFORM1FVPROC              glUniform1fv;
PFNGLUNIFORM2FVPROC              glUniform2fv;
PFNGLUNIFORM3FVPROC              glUniform3fv;
PFNGLUNIFORM4FVPROC              glUniform4fv;
PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv;

PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
PFNGLDRAWRANGEELEMENTSPROC       glDrawRangeElements;

PFNGLGENBUFFERSPROC              glGenBuffers;
PFNGLDELETEBUFFERSPROC           glDeleteBuffers;
PFNGLBINDBUFFERPROC              glBindBuffer;
PFNGLBUFFERDATAPROC              glBufferData;
PFNGLMAPBUFFERPROC               glMapBuffer;
PFNGLUNMAPBUFFERPROC             glUnmapBuffer;

PFNGLGENRENDERBUFFERSPROC        glGenRenderbuffers;
PFNGLDELETERENDERBUFFERSPROC     glDeleteRenderbuffers;
PFNGLBINDRENDERBUFFERPROC        glBindRenderbuffer;
PFNGLRENDERBUFFERSTORAGEPROC     glRenderbufferStorage;
PFNGLGENFRAMEBUFFERSPROC         glGenFramebuffers;
PFNGLDELETEFRAMEBUFFERSPROC      glDeleteFramebuffers;
PFNGLBINDFRAMEBUFFERPROC         glBindFramebuffer;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
PFNGLFRAMEBUFFERTEXTURE2DPROC    glFramebufferTexture2D;
PFNGLCHECKFRAMEBUFFERSTATUSPROC  glCheckFramebufferStatus;
PFNGLBLITFRAMEBUFFERPROC         glBlitFramebuffer;
PFNGLDRAWBUFFERSPROC             glDrawBuffers;

PFNGLCREATESHADERPROC            glCreateShader;
PFNGLDELETESHADERPROC            glDeleteShader;
PFNGLSHADERSOURCEPROC            glShaderSource;
PFNGLCOMPILESHADERPROC           glCompileShader;
PFNGLGETSHADERIVPROC             glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog;
PFNGLCREATEPROGRAMPROC           glCreateProgram;
PFNGLDELETEPROGRAMPROC           glDeleteProgram;
PFNGLATTACHSHADERPROC            glAttachShader;
PFNGLDETACHSHADERPROC            glDetachShader;
PFNGLLINKPROGRAMPROC             glLinkProgram;
PFNGLGETPROGRAMIVPROC            glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog;
PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation;
PFNGLBINDATTRIBLOCATIONPROC      glBindAttribLocation;
PFNGLUSEPROGRAMPROC              glUseProgram;

PFNGLACTIVETEXTUREPROC           glActiveTexture;
PFNGLCOMPRESSEDTEXIMAGE2DPROC    glCompressedTexImage2D;
PFNGLGETCOMPRESSEDTEXIMAGEPROC   glGetCompressedTexImage;
PFNGLGENERATEMIPMAPPROC          glGenerateMipmap;

PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays;
PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
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

  System::error( 1, "GL error `%s' at %s:%d: %s", message, file, line, function );
}

#endif

void glInit()
{
#ifdef _WIN32
  OZ_REGISTER_GLFUNC( glUniform1i               );
  OZ_REGISTER_GLFUNC( glUniform2i               );
  OZ_REGISTER_GLFUNC( glUniform3i               );
  OZ_REGISTER_GLFUNC( glUniform4i               );
  OZ_REGISTER_GLFUNC( glUniform1iv              );
  OZ_REGISTER_GLFUNC( glUniform2iv              );
  OZ_REGISTER_GLFUNC( glUniform3iv              );
  OZ_REGISTER_GLFUNC( glUniform4iv              );
  OZ_REGISTER_GLFUNC( glUniform1f               );
  OZ_REGISTER_GLFUNC( glUniform2f               );
  OZ_REGISTER_GLFUNC( glUniform3f               );
  OZ_REGISTER_GLFUNC( glUniform4f               );
  OZ_REGISTER_GLFUNC( glUniform1fv              );
  OZ_REGISTER_GLFUNC( glUniform2fv              );
  OZ_REGISTER_GLFUNC( glUniform3fv              );
  OZ_REGISTER_GLFUNC( glUniform4fv              );
  OZ_REGISTER_GLFUNC( glUniformMatrix4fv        );

  OZ_REGISTER_GLFUNC( glEnableVertexAttribArray );
  OZ_REGISTER_GLFUNC( glVertexAttribPointer     );
  OZ_REGISTER_GLFUNC( glDrawRangeElements       );

  OZ_REGISTER_GLFUNC( glGenBuffers              );
  OZ_REGISTER_GLFUNC( glDeleteBuffers           );
  OZ_REGISTER_GLFUNC( glBindBuffer              );
  OZ_REGISTER_GLFUNC( glBufferData              );
  OZ_REGISTER_GLFUNC( glMapBuffer               );
  OZ_REGISTER_GLFUNC( glUnmapBuffer             );

  OZ_REGISTER_GLFUNC( glGenRenderbuffers        );
  OZ_REGISTER_GLFUNC( glDeleteRenderbuffers     );
  OZ_REGISTER_GLFUNC( glBindRenderbuffer        );
  OZ_REGISTER_GLFUNC( glRenderbufferStorage     );
  OZ_REGISTER_GLFUNC( glGenFramebuffers         );
  OZ_REGISTER_GLFUNC( glDeleteFramebuffers      );
  OZ_REGISTER_GLFUNC( glBindFramebuffer         );
  OZ_REGISTER_GLFUNC( glFramebufferRenderbuffer );
  OZ_REGISTER_GLFUNC( glFramebufferTexture2D    );
  OZ_REGISTER_GLFUNC( glCheckFramebufferStatus  );
  OZ_REGISTER_GLFUNC( glBlitFramebuffer         );
  OZ_REGISTER_GLFUNC( glDrawBuffers             );

  OZ_REGISTER_GLFUNC( glCreateShader            );
  OZ_REGISTER_GLFUNC( glDeleteShader            );
  OZ_REGISTER_GLFUNC( glShaderSource            );
  OZ_REGISTER_GLFUNC( glCompileShader           );
  OZ_REGISTER_GLFUNC( glGetShaderiv             );
  OZ_REGISTER_GLFUNC( glGetShaderInfoLog        );
  OZ_REGISTER_GLFUNC( glCreateProgram           );
  OZ_REGISTER_GLFUNC( glDeleteProgram           );
  OZ_REGISTER_GLFUNC( glAttachShader            );
  OZ_REGISTER_GLFUNC( glDetachShader            );
  OZ_REGISTER_GLFUNC( glLinkProgram             );
  OZ_REGISTER_GLFUNC( glGetProgramiv            );
  OZ_REGISTER_GLFUNC( glGetProgramInfoLog       );
  OZ_REGISTER_GLFUNC( glGetUniformLocation      );
  OZ_REGISTER_GLFUNC( glBindAttribLocation      );
  OZ_REGISTER_GLFUNC( glUseProgram              );

  OZ_REGISTER_GLFUNC( glActiveTexture           );
  OZ_REGISTER_GLFUNC( glCompressedTexImage2D    );
  OZ_REGISTER_GLFUNC( glGetCompressedTexImage   );
  OZ_REGISTER_GLFUNC( glGenerateMipmap          );

  OZ_REGISTER_GLFUNC( glGenVertexArrays         );
  OZ_REGISTER_GLFUNC( glDeleteVertexArrays      );
  OZ_REGISTER_GLFUNC( glBindVertexArray         );
#endif
}

}
}
