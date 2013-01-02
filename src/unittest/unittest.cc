/*
 * liboz - OpenZone Core Library.
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
 * @file unittest/unittest.cc
 */

#include "unittest.hh"

#include <cstdlib>

using namespace oz;

bool hasPassed = true;

bool Foo::allowCopy     = true;
bool Foo::allowMove     = true;
bool Foo::allowEqualsOp = true;
bool Foo::allowLessOp   = true;

#ifdef __native_client__
static int nacl_main()
#else
int main()
#endif
{
  Log() << "Unittest began\n";

  System::init();
  Math::seed( 42 );

  test_common();
  test_iterables();
  test_arrays();

  test_Alloc();

  test_String();

  Log() << ( hasPassed ? "Unittest PASSED\n" : "Unittest FAILED\n" );
  return EXIT_SUCCESS;
}

#ifdef __native_client__

void MainInstance::mainThreadMain( void* )
{
  nacl_main();
}

MainInstance::MainInstance( PP_Instance instance_ ) :
  pp::Instance( instance_ )
{
  System::instance = this;
}

MainInstance::~MainInstance()
{
  if( mainThread.isValid() ) {
    mainThread.join();
  }
}

bool MainInstance::Init( uint32_t, const char**, const char** )
{
  return true;
}

void MainInstance::DidChangeView( const pp::View& )
{
  if( !mainThread.isValid() ) {
    mainThread.start( "main", Thread::JOINABLE, mainThreadMain, nullptr );
  }
}

void MainInstance::DidChangeView( const pp::Rect&, const pp::Rect& )
{
  PP_NOTREACHED();
}

pp::Instance* MainModule::CreateInstance( PP_Instance instance )
{
  return new MainInstance( instance );
}

namespace pp
{

pp::Module* CreateModule()
{
  return new MainModule();
}

}

#endif
