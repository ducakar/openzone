/*
 *  Nirvana.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "nirvana/Nirvana.hpp"

#include "matrix/Matrix.hpp"
#include "matrix/BotClass.hpp"
#include "nirvana/Lua.hpp"

#include "nirvana/LuaMind.hpp"
#include "nirvana/RandomMind.hpp"

#define OZ_REGISTER_MINDCLASS( name ) \
  mindClasses.add( #name, MindCtor( &name##Mind::create, &name##Mind::read ) )

namespace oz
{
namespace nirvana
{

  Nirvana nirvana;

  void Nirvana::sync()
  {
    // remove minds of removed bots
    for( auto i = minds.iter(); i.isValid(); ) {
      Mind* mind = i;
      ++i;

      if( world.objects[mind->botIndex] == null ) {
        minds.remove( mind );
        delete mind;
      }
    }
    // add minds for new bots
    foreach( i, synapse.addedObjects.citer() ) {
      const Object* obj = world.objects[*i];

      if( obj != null && ( obj->flags & Object::BOT_BIT ) ) {
        const Bot* bot = static_cast<const Bot*>( obj );
        const BotClass* clazz = static_cast<const BotClass*>( bot->type );

        MindCtor* value = mindClasses.find( clazz->mindType );
        if( value != null ) {
          minds.add( value->create( bot->index ) );
        }
        else if( !clazz->mindType.isEmpty() ) {
          throw Exception( "Invalid mind type" );
        }
      }
    }
  }

  void Nirvana::update()
  {
    int count = 0;
    foreach( mind, minds.iter() ) {
      const Bot* bot = static_cast<const Bot*>( world.objects[mind->botIndex] );
      assert( bot != null && ( bot->flags & Object::BOT_BIT ) );

      if( ( ~bot->state & Bot::PLAYER_BIT ) &&
          ( ( mind->flags & Mind::FORCE_UPDATE_BIT ) || count % UPDATE_INTERVAL == updateModulo ) )
      {
        mind->update();
      }
      ++count;
    }
    updateModulo = ( updateModulo + 1 ) % UPDATE_INTERVAL;
  }

  int Nirvana::run( void* )
  {
    uint timeBegin;

    try{
#ifndef OZ_MINGW
      nirvana.sync();
#endif

      // prepare semaphores for the first world update
      SDL_SemPost( matrix.semaphore );
      SDL_SemWait( nirvana.semaphore );

      while( nirvana.isAlive ) {
        timeBegin = SDL_GetTicks();

#ifndef OZ_MINGW
        // FIXME freezes on Windows, works fine on Wine?
        nirvana.sync();

        // update minds
        nirvana.update();
#endif

        timer.nirvanaMillis += SDL_GetTicks() - timeBegin;

        // notify matrix it can proceed changing the world
        SDL_SemPost( matrix.semaphore );
        // wait until world has changed
        SDL_SemWait( nirvana.semaphore );
      }
    }
    catch( const Exception& e ) {
      log.resetIndent();
      log.println();
      log.println( "EXCEPTION: %s:%d: %s", e.file, e.line, e.message );

      if( log.isFile() ) {
        fprintf( stderr, "EXCEPTION: %s:%d: %s\n", e.file, e.line, e.message );
      }
      abort();
    }
    return 0;
  }

  void Nirvana::init()
  {
    log.println( "Initialising Nirvana {" );
    log.indent();

    lua.init();

    OZ_REGISTER_MINDCLASS( Lua );
    OZ_REGISTER_MINDCLASS( Random );

    semaphore = SDL_CreateSemaphore( 0 );

    log.unindent();
    log.println( "}" );
  }

  void Nirvana::free()
  {
    log.println( "Freeing Nirvana {" );
    log.indent();

    SDL_DestroySemaphore( semaphore );

    minds.free();
    mindClasses.clear();
    lua.free();

    log.unindent();
    log.println( "}" );
  }

  void Nirvana::load( InputStream* istream )
  {
    log.print( "Loading Nirvana ..." );

    if( istream != null ) {
      read( istream );
    }

    log.printEnd( " OK" );
  }

  void Nirvana::unload( OutputStream* ostream )
  {
    log.print( "Unloading Nirvana ..." );

    if( ostream != null ) {
      write( ostream );
    }
    minds.free();

    log.printEnd( " OK" );
  }

  void Nirvana::start()
  {
    log.print( "Starting Nirvana thread ..." );

    updateModulo = 0;
    isAlive = true;
    thread = SDL_CreateThread( run, null );

    log.printEnd( " OK" );
  }

  void Nirvana::stop()
  {
    log.print( "Stopping Nirvana thread ..." );

    isAlive = false;

    SDL_SemPost( semaphore );
    SDL_WaitThread( thread, null );

    thread = null;

    log.printEnd( " OK" );
  }

  void Nirvana::read( InputStream* istream )
  {
    String typeName;
    int nMinds = istream->readInt();

    for( int i = 0; i < nMinds; ++i ) {
      istream->readString( typeName );
      if( !typeName.isEmpty() ) {
        minds.add( mindClasses.get( typeName ).read( istream ) );
      }
    }
  }

  void Nirvana::write( OutputStream* ostream ) const
  {
    ostream->writeInt( minds.length() );

    foreach( mind, minds.citer() ) {
      ostream->writeString( mind->type() );
      mind->write( ostream );
    }
  }

}
}
