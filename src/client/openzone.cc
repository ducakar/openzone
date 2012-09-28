/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file client/openzone.cc
 */

#include <stable.hh>
#include <client/openzone.hh>

#include <client/Client.hh>
#include <client/NaCl.hh>

#if defined( OZ_JNI )

#include <jni.h>

using namespace oz;

extern "C" JNIEXPORT
void JNICALL Java_OpenZone_main( JNIEnv* env, jclass, jobjectArray args )
{
  System::init( System::EXCEPTIONS_BIT );

  int exitCode = EXIT_FAILURE;

  Log::printRaw( "OpenZone " OZ_VERSION "\n"
                 "Copyright © 2002-2012 Davorin Učakar\n"
                 "This program comes with ABSOLUTELY NO WARRANTY.\n"
                 "This is free software, and you are welcome to redistribute it\n"
                 "under certain conditions; See COPYING file for details.\n\n" );

  int   argc = min( env->GetArrayLength( args ), 255 );
  char* argv[256];

  argv[0] = const_cast<char*>( "OpenZone" );

  for( int i = 0; i < argc; ++i ) {
    jobject     obj = (jstring) env->GetObjectArrayElement( args, i );
    const char* s   = env->GetStringUTFChars( static_cast<jstring>( obj ), nullptr );

    argv[i + 1] = const_cast<char*>( s );

  }

  exitCode = client::client.init( 1 + argc, argv );

  if( exitCode == EXIT_SUCCESS ) {
    exitCode = client::client.main();
  }

  client::client.shutdown();

  if( Alloc::count != 0 ) {
    Log::verboseMode = true;
    bool isOutput = Alloc::printLeaks();
    Log::verboseMode = false;

    if( isOutput ) {
      Log::println( "There are some memory leaks. See '%s' for details.", Log::logFile() );
    }
  }
}

#elif defined( __native_client__ )

#include <SDL/SDL_nacl.h>

using namespace oz;

extern "C"
void alSetPpapiInfo( PP_Instance, PPB_GetInterface );

namespace oz
{
namespace client
{

void MainInstance::mainThreadMain( void* )
{
  int exitCode = EXIT_FAILURE;

  Log::printRaw( "OpenZone " OZ_VERSION "\n"
                 "Copyright © 2002-2012 Davorin Učakar\n"
                 "This program comes with ABSOLUTELY NO WARRANTY.\n"
                 "This is free software, and you are welcome to redistribute it\n"
                 "under certain conditions; See COPYING file for details.\n\n" );

  NaCl::post( "init:" );

  char  argv0[] = "openzone";
  char* argv[]  = { argv0 };

  exitCode = client::client.init( 1, argv );

  if( exitCode == EXIT_SUCCESS ) {
    exitCode = client::client.main();
  }

  client::client.shutdown();

  Alloc::printLeaks();

  NaCl::post( "quit:" );
}

MainInstance::MainInstance( PP_Instance instance_ ) :
  pp::Instance( instance_ ), pp::MouseLock( this ), fullscreen( this )
{
  System::module   = pp::Module::Get();
  System::instance = this;
  System::core     = pp::Module::Get()->core();
  System::init();

  NaCl::init();

  RequestInputEvents( PP_INPUTEVENT_CLASS_KEYBOARD | PP_INPUTEVENT_CLASS_MOUSE |
                      PP_INPUTEVENT_CLASS_WHEEL );
}

MainInstance::~MainInstance()
{
  if( mainThread.isValid() ) {
    mainThread.join();
  }

  NaCl::free();
}

bool MainInstance::Init( uint32_t, const char**, const char** )
{
  return true;
}

void MainInstance::DidChangeView( const pp::View& view )
{
  int width  = view.GetRect().width();
  int height = view.GetRect().height();

  if( width == NaCl::width && height == NaCl::height ) {
    return;
  }

  NaCl::width  = width;
  NaCl::height = height;

  if( !mainThread.isValid() ) {
    SDL_NACL_SetInstance( pp_instance(), NaCl::width, NaCl::height );
    alSetPpapiInfo( pp_instance(), System::module->get_browser_interface() );

    mainThread.start( "main", mainThreadMain, this );
  }
}

void MainInstance::DidChangeView( const pp::Rect&, const pp::Rect& )
{
  PP_NOTREACHED();
}

void MainInstance::HandleMessage( const pp::Var& message )
{
  NaCl::push( message.AsString().c_str() );
}

bool MainInstance::HandleInputEvent( const pp::InputEvent& event )
{
  switch( event.GetType() ) {
    case PP_INPUTEVENT_TYPE_MOUSEMOVE: {
      pp::MouseInputEvent mouseEvent( event );
      pp::Point move = mouseEvent.GetMovement();

      NaCl::moveX += move.x();
      NaCl::moveY += move.y();
      break;
    }
    case PP_INPUTEVENT_TYPE_WHEEL: {
      pp::WheelInputEvent wheelEvent( event );
      pp::FloatPoint move = wheelEvent.GetDelta();

      NaCl::moveZ += int( Math::round( move.x() ) );
      NaCl::moveW += int( Math::round( move.y() ) );
      break;
    }
    case PP_INPUTEVENT_TYPE_MOUSEDOWN: {
      if( !NaCl::hasFocus ) {
        LockMouse( pp::CompletionCallback( &onMouseLocked, this ) );
        return true;
      }
      break;
    }
    case PP_INPUTEVENT_TYPE_KEYDOWN: {
      pp::KeyboardInputEvent keyEvent( event );

      if( ( keyEvent.GetKeyCode() == 122 || keyEvent.GetKeyCode() == 13 ) &&
          event.GetModifiers() == 0 )
      {
        fullscreen.SetFullscreen( !fullscreen.IsFullscreen() );
        return true;
      }
      break;
    }
    default: {
      break;
    }
  }

  SDL_NACL_PushEvent( event );
  return true;
}

void MainInstance::MouseLockLost()
{
  NaCl::hasFocus = false;
  fullscreen.SetFullscreen( false );
}

void MainInstance::onMouseLocked( void*, int result )
{
  NaCl::hasFocus = result == PP_OK;
}

pp::Instance* MainModule::CreateInstance( PP_Instance instance )
{
  return new MainInstance( instance );
}

}
}

namespace pp
{

pp::Module* CreateModule()
{
  return new oz::client::MainModule();
}

}

#else

using namespace oz;

int main( int argc, char** argv )
{
  System::init();

  int exitCode = EXIT_FAILURE;

  Log::printRaw( "OpenZone " OZ_VERSION "\n"
                 "Copyright © 2002-2012 Davorin Učakar\n"
                 "This program comes with ABSOLUTELY NO WARRANTY.\n"
                 "This is free software, and you are welcome to redistribute it\n"
                 "under certain conditions; See COPYING file for details.\n\n" );

  exitCode = client::client.init( argc, argv );

  if( exitCode == EXIT_SUCCESS ) {
    exitCode = client::client.main();
  }

  client::client.shutdown();

  if( Alloc::count != 0 ) {
    Log::verboseMode = true;
    bool isOutput = Alloc::printLeaks();
    Log::verboseMode = false;

    if( isOutput ) {
      Log::println( "There are some memory leaks. See '%s' for details.", Log::logFile() );
    }
  }

  return exitCode;
}

#endif
