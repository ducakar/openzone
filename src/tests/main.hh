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

#ifdef __native_client__

#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/graphics_3d.h>
#include <ppapi/cpp/fullscreen.h>
#include <ppapi/cpp/input_event.h>
#include <ppapi/cpp/mouse_lock.h>

#define main( argc, argv ) ozMain( argc, argv )

int ozMain( int argc, char** argv );

namespace oz
{

class MainInstance : public pp::Instance, public pp::MouseLock
{
  private:

    pp::Fullscreen fullscreen;
    pp::Graphics3D context;
    bool           isContextBound;
    bool           isMouseLocked;
    pthread_t      mainThread;

    static void* mainThreadMain( void* );

  public:

    explicit MainInstance( PP_Instance instance );
    ~MainInstance();

    bool Init( uint32_t argc, const char** argn, const char** argv ) override;
    void DidChangeView( const pp::View& view ) override;
    void DidChangeView( const pp::Rect& position, const pp::Rect& clip ) override;
    bool HandleInputEvent( const pp::InputEvent& event ) override;
    void MouseLockLost() override;

    static void Empty( void*, int );
    static void DidMouseLock( void* data, int result );

};

class MainModule : public pp::Module
{
  public:

    pp::Instance* CreateInstance( PP_Instance instance ) override;

};

}

namespace pp
{

pp::Module* CreateModule();

}

#endif // __native_client__