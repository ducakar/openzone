/*
 *  common.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/common.hpp"

#include <GL/gl.h>

namespace oz
{
namespace client
{

  const TexCoord TexCoord::ZERO = TexCoord( 0.0f, 0.0f );

#ifdef OZ_WINDOWS

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

  PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
  PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays;
  PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;

  PFNGLGENBUFFERSPROC              glGenBuffers;
  PFNGLDELETEBUFFERSPROC           glDeleteBuffers;
  PFNGLBINDBUFFERPROC              glBindBuffer;
  PFNGLBUFFERDATAPROC              glBufferData;
  PFNGLMAPBUFFERPROC               glMapBuffer;
  PFNGLUNMAPBUFFERPROC             glUnmapBuffer;

  PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
  PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;

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
//   PFNGLBINDFRAGDATALOCATIONPROC    glBindFragDataLocation;
  PFNGLUSEPROGRAMPROC              glUseProgram;

  PFNGLACTIVETEXTUREPROC           wglActiveTexture;
  PFNGLCOMPRESSEDTEXIMAGE2DPROC    wglCompressedTexImage2D;
  PFNGLDRAWRANGEELEMENTSPROC       wglDrawRangeElements;

#endif

}
}
