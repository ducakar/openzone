/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file tests/mono.cc
 */

#include <ozCore/ozCore.hh>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#define OZ_MONO_FUNCTION( func ) \
  mono_add_internal_call( "OpenZone." #func, reinterpret_cast<const void*>( &func ) )

using namespace oz;

static MonoDomain*   monoDomain;
static MonoAssembly* monoAssembly;

struct Object;
static DArray<Object> objects( 100 );

struct Object
{
  int   index;
  Point p;
  Vec3  velocity;
  float life;

  static void getP( MonoObject*, int index, float* x, float* y, float* z )
  {
    const Point& p = objects[index].p;

    *x = p.x;
    *y = p.y;
    *z = p.z;
  }

  static void setP( MonoObject*, int index, float x, float y, float z )
  {
    Point& p = objects[index].p;

    p.x = x;
    p.y = y;
    p.z = z;
  }

  static void getVelocity( MonoObject*, int index, float* x, float* y, float* z )
  {
    const Vec3& velocity = objects[index].velocity;

    *x = velocity.x;
    *y = velocity.y;
    *z = velocity.z;
  }

  static void setVelocity( MonoObject*, int index, float x, float y, float z )
  {
    Vec3& velocity = objects[index].velocity;

    velocity.x = x;
    velocity.y = y;
    velocity.z = z;
  }

  static float getLife( MonoObject*, int index )
  {
    return objects[index].life;
  }

  static void setLife( MonoObject*, int index, float value )
  {
    objects[index].life = value;
  }
};

int main( int argc, char** argv )
{
  System::init();

  monoDomain   = mono_jit_init( "openzone" );
  monoAssembly = mono_domain_assembly_open( monoDomain, "MonoTest.exe" );

  // Reinstall signal handlers Mono has overwritten.
  System::threadInit();

  hard_assert( monoAssembly != nullptr );

  OZ_MONO_FUNCTION( Object::getP );
  OZ_MONO_FUNCTION( Object::setP );
  OZ_MONO_FUNCTION( Object::getVelocity );
  OZ_MONO_FUNCTION( Object::setVelocity );
  OZ_MONO_FUNCTION( Object::getLife );
  OZ_MONO_FUNCTION( Object::setLife );

  objects[42].p = Point( 1.0f, 2.0f, 3.0f );
  objects[42].life = 3.14f;

  mono_jit_exec( monoDomain, monoAssembly, argc, argv );
  mono_jit_cleanup( monoDomain );
  return 0;
}
