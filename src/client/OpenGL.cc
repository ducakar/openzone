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

#define OZ_REGISTER_GLFUNC( func, type ) \
  *reinterpret_cast<void**>( &func ) = SDL_GL_GetProcAddress( #func ); \
  if( func == null ) { \
    throw Exception( "Failed to link OpenGL function '" #func "'" ); \
  }

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

# ifndef OZ_GL_COMPATIBLE
PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays;
PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
# endif
#endif

#ifndef NDEBUG

void glCheckError( const char* file, int line, const char* function )
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
    case GL_STACK_OVERFLOW: {
      message = "GL_STACK_OVERFLOW";
      break;
    }
    case GL_OUT_OF_MEMORY: {
      message = "GL_OUT_OF_MEMORY";
      break;
    }
    case GL_TABLE_TOO_LARGE: {
      message = "GL_TABLE_TOO_LARGE";
      break;
    }
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
  OZ_REGISTER_GLFUNC( glUniform1i,               PFNGLUNIFORM1IPROC               );
  OZ_REGISTER_GLFUNC( glUniform2i,               PFNGLUNIFORM2IPROC               );
  OZ_REGISTER_GLFUNC( glUniform3i,               PFNGLUNIFORM3IPROC               );
  OZ_REGISTER_GLFUNC( glUniform4i,               PFNGLUNIFORM4IPROC               );
  OZ_REGISTER_GLFUNC( glUniform1iv,              PFNGLUNIFORM1IVPROC              );
  OZ_REGISTER_GLFUNC( glUniform2iv,              PFNGLUNIFORM2IVPROC              );
  OZ_REGISTER_GLFUNC( glUniform3iv,              PFNGLUNIFORM3IVPROC              );
  OZ_REGISTER_GLFUNC( glUniform4iv,              PFNGLUNIFORM4IVPROC              );
  OZ_REGISTER_GLFUNC( glUniform1f,               PFNGLUNIFORM1FPROC               );
  OZ_REGISTER_GLFUNC( glUniform2f,               PFNGLUNIFORM2FPROC               );
  OZ_REGISTER_GLFUNC( glUniform3f,               PFNGLUNIFORM3FPROC               );
  OZ_REGISTER_GLFUNC( glUniform4f,               PFNGLUNIFORM4FPROC               );
  OZ_REGISTER_GLFUNC( glUniform1fv,              PFNGLUNIFORM1FVPROC              );
  OZ_REGISTER_GLFUNC( glUniform2fv,              PFNGLUNIFORM2FVPROC              );
  OZ_REGISTER_GLFUNC( glUniform3fv,              PFNGLUNIFORM3FVPROC              );
  OZ_REGISTER_GLFUNC( glUniform4fv,              PFNGLUNIFORM4FVPROC              );
  OZ_REGISTER_GLFUNC( glUniformMatrix4fv,        PFNGLUNIFORMMATRIX4FVPROC        );

  OZ_REGISTER_GLFUNC( glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC );
  OZ_REGISTER_GLFUNC( glVertexAttribPointer,     PFNGLVERTEXATTRIBPOINTERPROC     );
  OZ_REGISTER_GLFUNC( glDrawRangeElements,       PFNGLDRAWRANGEELEMENTSPROC       );

  OZ_REGISTER_GLFUNC( glGenBuffers,              PFNGLGENBUFFERSPROC              );
  OZ_REGISTER_GLFUNC( glDeleteBuffers,           PFNGLDELETEBUFFERSPROC           );
  OZ_REGISTER_GLFUNC( glBindBuffer,              PFNGLBINDBUFFERPROC              );
  OZ_REGISTER_GLFUNC( glBufferData,              PFNGLBUFFERDATAPROC              );
  OZ_REGISTER_GLFUNC( glMapBuffer,               PFNGLMAPBUFFERPROC               );
  OZ_REGISTER_GLFUNC( glUnmapBuffer,             PFNGLUNMAPBUFFERPROC             );

  OZ_REGISTER_GLFUNC( glGenRenderbuffers,        PFNGLGENRENDERBUFFERSPROC        );
  OZ_REGISTER_GLFUNC( glDeleteRenderbuffers,     PFNGLDELETERENDERBUFFERSPROC     );
  OZ_REGISTER_GLFUNC( glBindRenderbuffer,        PFNGLBINDRENDERBUFFERPROC        );
  OZ_REGISTER_GLFUNC( glRenderbufferStorage,     PFNGLRENDERBUFFERSTORAGEPROC     );
  OZ_REGISTER_GLFUNC( glGenFramebuffers,         PFNGLGENFRAMEBUFFERSPROC         );
  OZ_REGISTER_GLFUNC( glDeleteFramebuffers,      PFNGLDELETEFRAMEBUFFERSPROC      );
  OZ_REGISTER_GLFUNC( glBindFramebuffer,         PFNGLBINDFRAMEBUFFERPROC         );
  OZ_REGISTER_GLFUNC( glFramebufferRenderbuffer, PFNGLFRAMEBUFFERRENDERBUFFERPROC );
  OZ_REGISTER_GLFUNC( glFramebufferTexture2D,    PFNGLFRAMEBUFFERTEXTURE2DPROC    );
  OZ_REGISTER_GLFUNC( glCheckFramebufferStatus,  PFNGLCHECKFRAMEBUFFERSTATUSPROC  );
  OZ_REGISTER_GLFUNC( glBlitFramebuffer,         PFNGLBLITFRAMEBUFFERPROC         );
  OZ_REGISTER_GLFUNC( glDrawBuffers,             PFNGLDRAWBUFFERSPROC             );

  OZ_REGISTER_GLFUNC( glCreateShader,            PFNGLCREATESHADERPROC            );
  OZ_REGISTER_GLFUNC( glDeleteShader,            PFNGLDELETESHADERPROC            );
  OZ_REGISTER_GLFUNC( glShaderSource,            PFNGLSHADERSOURCEPROC            );
  OZ_REGISTER_GLFUNC( glCompileShader,           PFNGLCOMPILESHADERPROC           );
  OZ_REGISTER_GLFUNC( glGetShaderiv,             PFNGLGETSHADERIVPROC             );
  OZ_REGISTER_GLFUNC( glGetShaderInfoLog,        PFNGLGETSHADERINFOLOGPROC        );
  OZ_REGISTER_GLFUNC( glCreateProgram,           PFNGLCREATEPROGRAMPROC           );
  OZ_REGISTER_GLFUNC( glDeleteProgram,           PFNGLDELETEPROGRAMPROC           );
  OZ_REGISTER_GLFUNC( glAttachShader,            PFNGLATTACHSHADERPROC            );
  OZ_REGISTER_GLFUNC( glDetachShader,            PFNGLDETACHSHADERPROC            );
  OZ_REGISTER_GLFUNC( glLinkProgram,             PFNGLLINKPROGRAMPROC             );
  OZ_REGISTER_GLFUNC( glGetProgramiv,            PFNGLGETPROGRAMIVPROC            );
  OZ_REGISTER_GLFUNC( glGetProgramInfoLog,       PFNGLGETPROGRAMINFOLOGPROC       );
  OZ_REGISTER_GLFUNC( glGetUniformLocation,      PFNGLGETUNIFORMLOCATIONPROC      );
  OZ_REGISTER_GLFUNC( glBindAttribLocation,      PFNGLBINDFRAGDATALOCATIONPROC    );
  OZ_REGISTER_GLFUNC( glUseProgram,              PFNGLUSEPROGRAMPROC              );

  OZ_REGISTER_GLFUNC( glActiveTexture,           PFNGLACTIVETEXTUREPROC           );
  OZ_REGISTER_GLFUNC( glCompressedTexImage2D,    PFNGLCOMPRESSEDTEXIMAGE2DPROC    );
  OZ_REGISTER_GLFUNC( glGetCompressedTexImage,   PFNGLGETCOMPRESSEDTEXIMAGEPROC   );
  OZ_REGISTER_GLFUNC( glGenerateMipmap,          PFNGLGENERATEMIPMAPPROC          );

# ifndef OZ_GL_COMPATIBLE
  OZ_REGISTER_GLFUNC( glGenVertexArrays,         PFNGLGENVERTEXARRAYSPROC         );
  OZ_REGISTER_GLFUNC( glDeleteVertexArrays,      PFNGLDELETEVERTEXARRAYSPROC      );
  OZ_REGISTER_GLFUNC( glBindVertexArray,         PFNGLBINDVERTEXARRAYPROC         );
# endif
#endif
}

}
}
