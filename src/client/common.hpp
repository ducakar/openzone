/*
 *  common.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#ifdef OZ_MINGW
# undef near
# undef far
#endif

namespace oz
{
namespace client
{

  struct TexCoord
  {
    static const TexCoord ZERO;

    float u;
    float v;

    OZ_ALWAYS_INLINE
    TexCoord()
    {}

    OZ_ALWAYS_INLINE
    explicit TexCoord( float u_, float v_ ) : u( u_ ), v( v_ )
    {}

    OZ_ALWAYS_INLINE
    bool operator == ( const TexCoord& tc ) const
    {
      return u == tc.u && v == tc.v;
    }

    OZ_ALWAYS_INLINE
    bool operator != ( const TexCoord& tc ) const
    {
      return u != tc.u || v != tc.v;
    }

    OZ_ALWAYS_INLINE
    operator const float* () const
    {
      return &u;
    }

    OZ_ALWAYS_INLINE
    operator float* ()
    {
      return &u;
    }

    OZ_ALWAYS_INLINE
    const float& operator [] ( int i ) const
    {
      hard_assert( 0 <= i && i < 2 );

      return ( &u )[i];
    }

    OZ_ALWAYS_INLINE
    float& operator [] ( int i )
    {
      hard_assert( 0 <= i && i < 2 );

      return ( &u )[i];
    }
  };

#ifndef OZ_OPENGL3
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

  extern PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
  extern PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays;
  extern PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;

  extern PFNGLGENBUFFERSPROC              glGenBuffers;
  extern PFNGLDELETEBUFFERSPROC           glDeleteBuffers;
  extern PFNGLBINDBUFFERPROC              glBindBuffer;
  extern PFNGLBUFFERDATAPROC              glBufferData;
  extern PFNGLMAPBUFFERPROC               glMapBuffer;
  extern PFNGLUNMAPBUFFERPROC             glUnmapBuffer;

  extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
  extern PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;

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
  extern PFNGLDRAWRANGEELEMENTSPROC       glDrawRangeElements;
#endif

  GLenum glGetError();
  ALenum alGetError();

}
}
