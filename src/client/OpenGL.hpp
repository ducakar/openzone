/*
 *  OpenGL.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#define GL_VERSION_1_2_DEPRECATED
#define GL_VERSION_1_3_DEPRECATED
#define GL_VERSION_1_4_DEPRECATED
#define GL_VERSION_1_5_DEPRECATED
#define GL_VERSION_2_0_DEPRECATED
#define GL_VERSION_2_1_DEPRECATED
#define GL_VERSION_3_0_DEPRECATED
#define GL_ARB_imaging_DEPRECATED
#define GL_ARB_framebuffer_object_DEPRECATED

#ifndef OZ_MINGW
# define GL_GLEXT_PROTOTYPES
#endif

#include <GL/gl.h>

// fix M$ crap from Windows headers
#ifdef OZ_MINGW
# undef PLANES
# undef near
# undef far
#endif

namespace oz
{
namespace client
{

#ifdef OZ_MINGW
  extern PFNGLUNIFORM1IPROC               glUniform1i;
  extern PFNGLUNIFORM2IPROC               glUniform2i;
  extern PFNGLUNIFORM3IPROC               glUniform3i;
  extern PFNGLUNIFORM4IPROC               glUniform4i;
  extern PFNGLUNIFORM1IVPROC              glUniform1iv;
  extern PFNGLUNIFORM2IVPROC              glUniform2iv;
  extern PFNGLUNIFORM3IVPROC              glUniform3iv;
  extern PFNGLUNIFORM4IVPROC              glUniform4iv;
  extern PFNGLUNIFORM1FPROC               glUniform1f;
  extern PFNGLUNIFORM2FPROC               glUniform2f;
  extern PFNGLUNIFORM3FPROC               glUniform3f;
  extern PFNGLUNIFORM4FPROC               glUniform4f;
  extern PFNGLUNIFORM1FVPROC              glUniform1fv;
  extern PFNGLUNIFORM2FVPROC              glUniform2fv;
  extern PFNGLUNIFORM3FVPROC              glUniform3fv;
  extern PFNGLUNIFORM4FVPROC              glUniform4fv;
  extern PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv;

  extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
  extern PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
  extern PFNGLDRAWRANGEELEMENTSPROC       glDrawRangeElements;

  extern PFNGLGENBUFFERSPROC              glGenBuffers;
  extern PFNGLDELETEBUFFERSPROC           glDeleteBuffers;
  extern PFNGLBINDBUFFERPROC              glBindBuffer;
  extern PFNGLBUFFERDATAPROC              glBufferData;
  extern PFNGLMAPBUFFERPROC               glMapBuffer;
  extern PFNGLUNMAPBUFFERPROC             glUnmapBuffer;

  extern PFNGLCREATESHADERPROC            glCreateShader;
  extern PFNGLDELETESHADERPROC            glDeleteShader;
  extern PFNGLSHADERSOURCEPROC            glShaderSource;
  extern PFNGLCOMPILESHADERPROC           glCompileShader;
  extern PFNGLGETSHADERIVPROC             glGetShaderiv;
  extern PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog;
  extern PFNGLCREATEPROGRAMPROC           glCreateProgram;
  extern PFNGLDELETEPROGRAMPROC           glDeleteProgram;
  extern PFNGLATTACHSHADERPROC            glAttachShader;
  extern PFNGLDETACHSHADERPROC            glDetachShader;
  extern PFNGLLINKPROGRAMPROC             glLinkProgram;
  extern PFNGLGETPROGRAMIVPROC            glGetProgramiv;
  extern PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog;
  extern PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation;
  extern PFNGLBINDATTRIBLOCATIONPROC      glBindAttribLocation;
  extern PFNGLUSEPROGRAMPROC              glUseProgram;

  extern PFNGLACTIVETEXTUREPROC           glActiveTexture;
  extern PFNGLCOMPRESSEDTEXIMAGE2DPROC    glCompressedTexImage2D;
  extern PFNGLGENERATEMIPMAPPROC          glGenerateMipmap;

# ifndef OZ_GL_COMPATIBLE
  extern PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
  extern PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays;
  extern PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
# endif
#endif

  GLenum glGetError();

}
}
