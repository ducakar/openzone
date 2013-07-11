/*
 * ozFactory - OpenZone Assets Builder Library.
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
 * @file ozFactory/TerraBuilder.cc
 */

#include "TerraBuilder.hh"

#include <noise/noise.h>

using namespace noise;

namespace oz
{

static const int ERROR_LENGTH = 1024;

static char       errorBuffer[ERROR_LENGTH] = {};
static List<Vec4> gradientPoints;

static uint colour( float height )
{
  if( gradientPoints.isEmpty() ) {
    return 0xff000000;
  }

  const Vec4  black( 0.0f, 0.0f, 0.0f, 0.0f );
  const Vec4* bottom = &gradientPoints.last();
  const Vec4* top    = &gradientPoints.last();
  float       t      = 0.0f;

  for( int i = 0; i < gradientPoints.length(); ++i ) {
    if( gradientPoints[i].w > height ) {
      bottom = &gradientPoints[ max( i - 1, 0 ) ];
      top    = &gradientPoints[i];

      if( bottom != top ) {
        t = ( height - bottom->w ) / ( top->w - bottom->w );
        t = clamp( t, 0.0f, 1.0f );
      }
      break;
    }
  }

  Vec4 colour = Math::mix( *bottom, *top, t );

  uint red   = uint( 255.0f * colour.x );
  uint green = uint( 255.0f * colour.y );
  uint blue  = uint( 255.0f * colour.z );

  return 0xff000000 | red | ( green << 8 ) | ( blue << 16 );
}

const char* TerraBuilder::getError()
{
  return errorBuffer;
}

void TerraBuilder::addGradientPoint( const Vec4& point )
{
  errorBuffer[0] = '\0';

  gradientPoints.add( point );
}

void TerraBuilder::clearGradient()
{
  errorBuffer[0] = '\0';

  gradientPoints.clear();
  gradientPoints.deallocate();
}

float* TerraBuilder::generateHeightmap( int width, int height )
{
  errorBuffer[0] = '\0';

  module::Billow mod;

  mod.SetSeed( 45 );
  mod.SetFrequency( 0.80 );
  mod.SetOctaveCount( 4 );
  mod.SetPersistence( 0.5 );

  float* heightmap = new float[width * height];
  double dWidth    = width;
  double dHeight   = height;

  for( int x = 0; x < width; ++x ) {
    for( int y = 0; y < height; ++y ) {
      heightmap[x * height + y] = float( mod.GetValue( x / dWidth, y / dHeight, 1.0 ) );
    }
  }
  return heightmap;
}

char* TerraBuilder::generateImage( int width, int height )
{
  errorBuffer[0] = '\0';

  module::Billow mod;

  mod.SetSeed( 45 );
  mod.SetFrequency( 2.0 );
  mod.SetOctaveCount( 4 );
  mod.SetPersistence( 0.5 );

  int    pitch   = ( ( width * 3 + 3 ) / 4 ) * 4;
  char*  image   = new char[height * pitch] {};
  double dWidth  = width;
  double dHeight = height;

  for( int y = 0; y < height; ++y ) {
    char* pixels = &image[y * pitch];

    for( int x = 0; x < width; ++x ) {
      float value       = float( mod.GetValue( x / dWidth, y / dHeight, 1.0 ) );
      uint  pixelColour = colour( value );

      pixels[0] = char( pixelColour & 0xff );
      pixels[1] = char( pixelColour >> 8 & 0xff );
      pixels[2] = char( pixelColour >> 16 & 0xff );
      pixels += 3;
    }
  }
  return image;
}

}
