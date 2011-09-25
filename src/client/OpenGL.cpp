/*
 *  OpenGL.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{

#ifdef OZ_MINGW
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
  PFNGLGENERATEMIPMAPPROC          glGenerateMipmap;

#ifndef OZ_GL_COMPATIBLE
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
        message = "UNKNOWN(" + String( int( result ) ) + ")";
        break;
      }
    }

    System::trap();
    System::abort( "GL error `%s' at %s:%d: %s", message, file, line, function );
  }

#endif

}
}
