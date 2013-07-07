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
 * @file ozEngine/GL.cc
 */

#include "GL.hh"

#ifdef _WIN32
# include <SDL.h>

# define OZ_DL_GLLOAD( func ) \
  *( void** ) &func = SDL_GL_GetProcAddress( #func ); \
  if( func == nullptr ) { \
    OZ_ERROR( "Failed to link OpenGL function: " #func ); \
  }
#endif

namespace oz
{

static const int DDSD_PITCH_BIT       = 0x00000008;
static const int DDSD_MIPMAPCOUNT_BIT = 0x00020000;
static const int DDSD_LINEARSIZE_BIT  = 0x00080000;

static const int DDPF_ALPHAPIXELS     = 0x00000001;
static const int DDPF_FOURCC          = 0x00000004;
static const int DDPF_RGB             = 0x00000040;

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

void GL::checkError( const char* function, const char* file, int line )
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

int GL::textureDataFromFile( const File& file, int bias )
{
  InputStream istream = file.inputStream();

  // Implementation is based on specifications from
  // http://msdn.microsoft.com/en-us/library/windows/desktop/bb943991%28v=vs.85%29.aspx.
  if( !istream.isAvailable() || !String::beginsWith( istream.begin(), "DDS " ) ) {
    return 0;
  }

  istream.readInt();
  istream.readInt();

  int flags  = istream.readInt();
  int height = istream.readInt();
  int width  = istream.readInt();
  int pitch  = istream.readInt();

  istream.readInt();

  int nMipmaps = istream.readInt();

  if( !( flags & DDSD_MIPMAPCOUNT_BIT ) ) {
    nMipmaps = 1;
  }

  hard_assert( nMipmaps >= 1 );
  bias = min( bias, nMipmaps - 1 );

  istream.seek( 4 + 76 );

  int pixelFlags = istream.readInt();
  int blockSize  = 1;

  char formatFourCC[4];
  istream.readChars( formatFourCC, 4 );

  int    bpp = istream.readInt();
  GLenum format;

  if( pixelFlags & DDPF_FOURCC ) {
    if( String::beginsWith( formatFourCC, "DXT1" ) ) {
      format    = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
      blockSize = 8;
    }
    else if( String::beginsWith( formatFourCC, "DXT3" ) ) {
      format    = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
      blockSize = 16;
    }
    else if( String::beginsWith( formatFourCC, "DXT5" ) ) {
      format    = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
      blockSize = 16;
    }
    else {
      return 0;
    }
  }
  else if( pixelFlags & DDPF_RGB ) {
    format    = pixelFlags & DDPF_ALPHAPIXELS ? GL_RGBA : GL_RGB;
    blockSize = 1;
  }
  else {
    return 0;
  }

  istream.seek( 4 + 124 );

  int mipmapWidth  = width;
  int mipmapHeight = height;
  int mipmapS3Size = pitch;

  if( nMipmaps == 1 ) {
    // Set GL_LINEAR minification filter instead of GL_NEAREST_MIPMAP_LINEAR as default for
    // non-mipmapped textures. Since those are usually used in UI, where texture repeating is not
    // desired in most cases, so we set GL_CLAMP_TO_EDGE by default.
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  }
  else {
    // Default minification filter in OpenGL is crappy GL_NEAREST_MIPMAP_LINEAR not regarding
    // whether texture actually has mipmaps.
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  }

  for( int i = 0; i < nMipmaps; ++i ) {
    if( pixelFlags & DDPF_FOURCC ) {
      const char* source = istream.forward( mipmapS3Size );

      if( i >= bias ) {
        glCompressedTexImage2D( GL_TEXTURE_2D, i - bias, format, mipmapWidth, mipmapHeight, 0,
                                mipmapS3Size, source );
      }
    }
    else {
      int         mipmapPitch = ( mipmapWidth * bpp + 7 ) / 8;
      int         mipmapSize  = mipmapHeight * mipmapPitch;
      const char* source      = istream.forward( mipmapSize );

      if( i >= bias ) {
        char* data      = new char[mipmapSize];
        char* dest      = data;
        int   pixelSize = bpp / 8;

        // Swap red and blue components and collapse gaps between scan lines.
        for( int j = 0; j < mipmapHeight; ++j ) {
          for( int k = 0; k < mipmapWidth; ++k ) {
            dest[0] = source[2];
            dest[1] = source[1];
            dest[2] = source[0];

            if( bpp == 32 ) {
              dest[3] = source[3];
            }

            dest   += pixelSize;
            source += pixelSize;
          }

          source += mipmapPitch - mipmapWidth * pixelSize;
        }

        glTexImage2D( GL_TEXTURE_2D, i - bias, int( format ), mipmapWidth, mipmapHeight, 0, format,
                      GL_UNSIGNED_BYTE, data );
        delete[] data;
      }
    }

    mipmapWidth  = max( 1, mipmapWidth / 2 );
    mipmapHeight = max( 1, mipmapHeight / 2 );
    mipmapS3Size = max( blockSize, mipmapS3Size / 4 );
  }

  return nMipmaps - bias;
}

static bool readShaderFile( const File& file, OutputStream* ostream, List<int>* fileOffsets )
{
  Buffer buffer = file.read();
  if( buffer.isEmpty() ) {
    return false;
  }

  InputStream  istream = buffer.inputStream();
  OutputStream cstream( 0 ); // Clean file contents, without #include directives.

  while( istream.isAvailable() ) {
    String line = istream.readLine();

    if( !line.beginsWith( "#include" ) ) {
      cstream.writeChars( line, line.length() );
    }
    else {
      // Insert included file BEFORE the current one (not at the position of #include directive).
      int startQuote = line.index( '"' );
      int endQuote   = line.lastIndex( '"' );

      if( startQuote > 0 && startQuote < endQuote ) {
        String dirName     = file.directory();
        String fileName    = line.substring( startQuote + 1, endQuote );
        File   includeFile = dirName.isEmpty() ? fileName : dirName + "/" + fileName;

        if( !readShaderFile( includeFile, ostream, fileOffsets ) ) {
          return false;
        }
      }
    }
    cstream.writeChar( '\n' );
  }

  fileOffsets->add( ostream->tell() );
  ostream->writeChars( cstream.begin(), cstream.tell() );
  return true;
}

bool GL::compileShaderFromFile( GLuint shader, const char* defines, const File& file )
{
  OutputStream ostream( 0 );
  List<int>    fileOffsets;

  if( !readShaderFile( file, &ostream, &fileOffsets ) ) {
    return false;
  }

  SList<const char*, 16> fileContents;
  SList<int, 16>         fileLengths;

  fileContents.add( defines );
  fileLengths.add( String::length( defines ) );

  for( int i = 0; i < fileOffsets.length(); ++i ) {
    fileContents.add( ostream.begin() + fileOffsets[i] );
    fileLengths.add( i == fileOffsets.length() - 1 ? ostream.tell() - fileOffsets[i] :
                                                     fileOffsets[i + 1] - fileOffsets[i] );
  }

  glShaderSource( shader, fileContents.length(), fileContents.begin(), fileLengths.begin() );
  glCompileShader( shader );

  int result;
  glGetShaderiv( shader, GL_COMPILE_STATUS, &result );

  OZ_GL_CHECK_ERROR();
  return result == GL_TRUE;
}

void GL::init()
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
