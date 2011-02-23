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

#define GL_GLEXT_PROTOTYPES

#define GL_VERSION_1_2_DEPRECATED
#define GL_VERSION_1_3_DEPRECATED
#define GL_VERSION_1_4_DEPRECATED
#define GL_VERSION_1_5_DEPRECATED
#define GL_VERSION_2_0_DEPRECATED
#define GL_VERSION_2_1_DEPRECATED
#define GL_VERSION_3_0_DEPRECATED

#define GL_ARB_imaging_DEPRECATED
#define GL_ARB_framebuffer_object_DEPRECATED

#ifdef OZ_WINDOWS

# include <SDL_opengl.h>

extern PFNGLACTIVETEXTUREPROC        glActiveTexture;
extern PFNGLCLIENTACTIVETEXTUREPROC  glClientActiveTexture;
extern PFNGLGENBUFFERSPROC           glGenBuffers;
extern PFNGLDELETEBUFFERSPROC        glDeleteBuffers;
extern PFNGLBINDBUFFERPROC           glBindBuffer;
extern PFNGLBUFFERDATAPROC           glBufferData;

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

    explicit TexCoord()
    {}

    explicit TexCoord( float u_, float v_ ) : u( u_ ), v( v_ )
    {}

    bool operator == ( const TexCoord& tc ) const
    {
      return u == tc.u && v == tc.v;
    }

    bool operator != ( const TexCoord& tc ) const
    {
      return u != tc.u || v != tc.v;
    }

    operator const float* () const
    {
      return &u;
    }

    operator float* ()
    {
      return &u;
    }

    const float& operator [] ( int i ) const
    {
      hard_assert( 0 <= i && i < 2 );

      return ( &u )[i];
    }

    float& operator [] ( int i )
    {
      hard_assert( 0 <= i && i < 2 );

      return ( &u )[i];
    }
  };

}
}
