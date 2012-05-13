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

#include "client/common.hh"

#define GL_VERSION_1_1_DEPRECATED
#define GL_VERSION_1_2_DEPRECATED
#define GL_VERSION_1_3_DEPRECATED
#define GL_VERSION_1_4_DEPRECATED
#define GL_VERSION_1_5_DEPRECATED
#define GL_VERSION_2_0_DEPRECATED
#define GL_VERSION_2_1_DEPRECATED
#define GL_VERSION_3_0_DEPRECATED
#define GL_ARB_imaging_DEPRECATED
#define GL_ARB_framebuffer_object_DEPRECATED

#define GL_EXT_texture_compression_s3tc
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3

#ifndef _WIN32
# define GL_GLEXT_PROTOTYPES
#endif

#include "NaClMainCall.hh"
#include "NaClGLContext.hh"

#ifdef OZ_GL_ES
# include <GLES2/gl2.h>
#else
# include <GL/gl.h>
#endif

namespace oz
{
namespace client
{

#ifdef _WIN32
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

extern PFNGLGENRENDERBUFFERSPROC        glGenRenderbuffers;
extern PFNGLDELETERENDERBUFFERSPROC     glDeleteRenderbuffers;
extern PFNGLBINDRENDERBUFFERPROC        glBindRenderbuffer;
extern PFNGLRENDERBUFFERSTORAGEPROC     glRenderbufferStorage;
extern PFNGLGENFRAMEBUFFERSPROC         glGenFramebuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC      glDeleteFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC         glBindFramebuffer;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC    glFramebufferTexture2D;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC  glCheckFramebufferStatus;
extern PFNGLBLITFRAMEBUFFERPROC         glBlitFramebuffer;
extern PFNGLDRAWBUFFERSPROC             glDrawBuffers;

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
extern PFNGLGETCOMPRESSEDTEXIMAGEPROC   glGetCompressedTexImage;
extern PFNGLGENERATEMIPMAPPROC          glGenerateMipmap;

extern PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
extern PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
#endif

#ifdef NDEBUG
# define OZ_GL_CHECK_ERROR() void( 0 )
#else
# define OZ_GL_CHECK_ERROR() oz::client::glCheckError( __FILE__, __LINE__, __PRETTY_FUNCTION__ )

void glCheckError( const char* file, int line, const char* function );

#endif

void glInit();

}
}
