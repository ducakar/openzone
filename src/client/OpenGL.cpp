/*
 *  OpenGL.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
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

  GLenum glGetError()
  {
    GLenum result = ::glGetError();

    if( result != GL_NO_ERROR ) {
      switch( result ) {
        case GL_INVALID_ENUM: {
          log.println( "GL: GL_INVALID_ENUM" );
          break;
        }
        case GL_INVALID_VALUE: {
          log.println( "GL: GL_INVALID_VALUE" );
          break;
        }
        case GL_INVALID_OPERATION: {
          log.println( "GL: GL_INVALID_OPERATION" );
          break;
        }
        case GL_STACK_OVERFLOW: {
          log.println( "GL: GL_STACK_OVERFLOW" );
          break;
        }
        case GL_OUT_OF_MEMORY: {
          log.println( "GL: GL_OUT_OF_MEMORY" );
          break;
        }
        case GL_TABLE_TOO_LARGE: {
          log.println( "GL: GL_TABLE_TOO_LARGE" );
          break;
        }
      }
    }

    return result;
  }

}
}
