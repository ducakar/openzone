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

static char                errorBuffer[ERROR_LENGTH] = {};
static module::Billow      plainsBase;
static module::ScaleBias   plainsFinal;
static module::RidgedMulti mountainsBase;
static module::ScaleBias   mountainsFinal;
static module::Perlin      terrainType;
static module::Select      combiner;
static module::Turbulence  turbulence;
static List<Vec4>          gradientPoints;
static bool                isInitialised = false;

static void ensureInitialised()
{
  if( isInitialised ) {
    return;
  }

  plainsFinal.SetSourceModule( 0, plainsBase );
  mountainsFinal.SetSourceModule( 0, mountainsBase );

  combiner.SetSourceModule( 0, plainsFinal );
  combiner.SetSourceModule( 1, mountainsFinal );
  combiner.SetControlModule( terrainType );

  turbulence.SetSourceModule( 0, combiner );

  isInitialised = true;
}

static float genHeight( double x, double y )
{
  double height = turbulence.GetValue( x, y, 1.0 );
  return float( height );
}

static uint getColour( float height )
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

bool TerraBuilder::setBounds( Module module, float bottomHeight, float topHeight )
{
  double bias  = ( bottomHeight + topHeight ) / 2.0;
  double scale = ( topHeight - bottomHeight ) / 2.0;

  switch( module ) {
    case PLAINS: {
      plainsFinal.SetBias( bias );
      plainsFinal.SetScale( scale );
      return true;
    }
    case MOUNTAINS: {
      mountainsFinal.SetBias( bias );
      mountainsFinal.SetScale( scale );
      return true;
    }
    default: {
      return false;
    }
  }
}

bool TerraBuilder::setSeed( Module module, int seed )
{
  switch( module ) {
    case PLAINS: {
      plainsBase.SetSeed( seed );
      return true;
    }
    case MOUNTAINS: {
      mountainsBase.SetSeed( seed );
      return true;
    }
    case COMBINER: {
      terrainType.SetSeed( seed );
      return true;
    }
    case TURBULENCE: {
      turbulence.SetSeed( seed );
      return true;
    }
  }
}

bool TerraBuilder::setOctaveCount( Module module, int count )
{
  switch( module ) {
    case PLAINS: {
      plainsBase.SetOctaveCount( count );
      return true;
    }
    case MOUNTAINS: {
      mountainsBase.SetOctaveCount( count );
      return true;
    }
    case COMBINER: {
      terrainType.SetOctaveCount( count );
      return true;
    }
    default: {
      return false;
    }
  }
}

bool TerraBuilder::setRoughness( Module module, int roughness )
{
  switch( module ) {
    case TURBULENCE: {
      turbulence.SetRoughness( roughness );
      return true;
    }
    default: {
      return false;
    }
  }
}

bool TerraBuilder::setFrequency( Module module, float frequency )
{
  switch( module ) {
    case PLAINS: {
      plainsBase.SetFrequency( frequency );
      return true;
    }
    case MOUNTAINS: {
      mountainsBase.SetFrequency( frequency );
      return true;
    }
    case COMBINER: {
      terrainType.SetFrequency( frequency );
      return true;
    }
    case TURBULENCE: {
      turbulence.SetFrequency( frequency );
      return true;
    }
  }
}

bool TerraBuilder::setPersistence( Module module, float persistence )
{
  switch( module ) {
    case PLAINS: {
      plainsBase.SetPersistence( persistence );
      return true;
    }
    case COMBINER: {
      terrainType.SetPersistence( persistence );
      return true;
    }
    default: {
      return false;
    }
  }
}

bool TerraBuilder::setPower( Module module, float power )
{
  switch( module ) {
    case TURBULENCE: {
      turbulence.SetPower( power );
      return true;
    }
    default: {
      return false;
    }
  }
}

void TerraBuilder::setMountainsControl( Module module )
{
  ensureInitialised();

  switch( module ) {
    case PLAINS: {
      combiner.SetControlModule( plainsBase );
      break;
    }
    case COMBINER: {
      combiner.SetControlModule( terrainType );
      break;
    }
    default: {
      break;
    }
  }
}

void TerraBuilder::setMountainsBounds( float lower, float upper )
{
  combiner.SetBounds( lower, upper );
}

void TerraBuilder::setEdgeFalloff( float falloff )
{
  combiner.SetEdgeFalloff( falloff );
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

  ensureInitialised();

  float* heightmap = new float[width * height];
  double dWidth    = width;
  double dHeight   = height;

  for( int x = 0; x < width; ++x ) {
    for( int y = 0; y < height; ++y ) {
      heightmap[x * height + y] = genHeight( x / dWidth, y / dHeight );
    }
  }
  return heightmap;
}

char* TerraBuilder::generateImage( int width, int height )
{
  errorBuffer[0] = '\0';

  ensureInitialised();

  int    pitch   = ( ( width * 3 + 3 ) / 4 ) * 4;
  char*  image   = new char[height * pitch] {};
  double dWidth  = width;
  double dHeight = height;

  for( int y = 0; y < height; ++y ) {
    char* pixels = &image[y * pitch];

    for( int x = 0; x < width; ++x ) {
      float value       = genHeight( x / dWidth, 1.0 - y / dHeight );
      uint  pixelColour = getColour( value );

      pixels[0] = char( pixelColour & 0xff );
      pixels[1] = char( pixelColour >> 8 & 0xff );
      pixels[2] = char( pixelColour >> 16 & 0xff );
      pixels += 3;
    }
  }
  return image;
}

}
