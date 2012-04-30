/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file ozmain/main.hh
 *
 * Platform-specific main function implementation.
 */

#pragma once

#include "oz/oz.hh"

int ozMain( int argc, char** argv );

#ifdef __native_client__

#include <ppapi/cpp/module.h>
#include <ppapi/cpp/instance.h>

namespace oz
{

class MainInstance : public pp::Instance
{
  private:

    pthread_t mainThread;
    int       width;
    int       height;

    static void* mainThreadMain( void* );

  public:

    explicit MainInstance( PP_Instance instance );
    virtual ~MainInstance();

    virtual void DidChangeView( const pp::View& view );
    virtual void DidChangeView( const pp::Rect& position, const pp::Rect& clip );
    virtual bool HandleInputEvent( const pp::InputEvent& event );
    virtual bool Init( uint32_t argc, const char* argn[], const char* argv[] );

};

class MainModule : public pp::Module
{
  public:

    virtual pp::Instance* CreateInstance( PP_Instance instance );

};

}

namespace pp
{

Module* CreateModule();

}

#else

int main( int argc, char** argv );

#endif
