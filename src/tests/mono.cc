/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

struct Test
{
  static MonoString* getString()
  {
    return mono_string_new( monoDomain, "Drek na palci" );
  }
};

int main( int argc, char** argv )
{
  System::init();

  monoDomain   = mono_jit_init( "openzone" );
  monoAssembly = mono_domain_assembly_open( monoDomain, "MonoTest.exe" );

  // Reinstall signal handlers Mono overwritten.
  System::threadInit();

  hard_assert( monoAssembly != nullptr );

  OZ_MONO_FUNCTION( Test::getString );
  mono_jit_exec( monoDomain, monoAssembly, argc, argv );
  mono_jit_cleanup( monoDomain );
  return 0;
}
